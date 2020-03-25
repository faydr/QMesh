/*
QMesh
Copyright (C) 2019 Daniel R. Fay

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mbed.h"
#include "peripherals.hpp"
#include "params.hpp"
#include "serial_data.hpp"
#include "fec.hpp"
#include "json_serial.hpp"
#include "mesh_protocol.hpp"
#include "mem_trace.hpp"
#include "Adafruit_SSD1306.h"
#include "SoftI2C.h"

RawSerial pc(USBTX, USBRX);
// Should be A_9 (TX) and PA_10 (RX) on the NUCLEO-F746ZG
RawSerial pc2(MBED_CONF_APP_ALT_UART_TX, MBED_CONF_APP_ALT_UART_RX);
JSONSerial rx_json_ser, tx_json_ser;
Thread tx_serial_thread(osPriorityNormal, 4096, NULL, "TX-SERIAL");
Thread rx_serial_thread(osPriorityNormal, 4096, NULL, "RX-SERIAL");
Thread mesh_protocol_thread(osPriorityRealtime, 4096, NULL, "MESH-FSM");
Thread rx_frame_thread(osPriorityNormal, 4096, NULL, "RX-FRAME");
Thread beacon_thread(osPriorityNormal, 4096, NULL, "BEACON");
Thread nv_log_thread(osPriorityNormal, 4096, NULL, "NV-LOG");
Thread button_thread(osPriorityNormal, 4096, NULL, "BUTTON");

system_state_t current_mode = BOOTING;
bool stay_in_management = false;

#define SLEEP_TIME                  500 // (msec)
#define PRINT_AFTER_N_LOOPS         20

void send_status(void);

DigitalIn user_button(USER_BUTTON);

SoftI2C oled_i2c(PB_8, PB_9);

#if 0
// an I2C sub-class that provides a constructed default
class I2CPreInit : public I2C
{
public:
    I2CPreInit(PinName sda, PinName scl) : I2C(sda, scl)
    {
        frequency(400000);
        start();
    };
};
I2CPreInit gI2C(PB_9, PB_8);
Adafruit_SSD1306_I2c oled(gI2C, NC);
#endif


// main() runs in its own thread in the OS
int main()
{
    led1.LEDBlink();
	ThisThread::sleep_for(1000);
	if(user_button) {
		led1.LEDFastBlink();
		ThisThread::sleep_for(1000);
		if(user_button) {
			rescue_filesystem();
		}
	}
	led1.LEDSolid();

    auto push_button = new PushButton(USER_BUTTON);
    button_thread.start(button_thread_fn);

    oled_i2c.frequency(100000);
    oled_i2c.start();

    Adafruit_SSD1306_I2c oled(oled_i2c, PD_13, 0x78, 32, 128);
    oled.clearDisplay();
    oled.printf("Welcome to QMesh\r\n");
    oled.display();
	
    // Start the WDT thread
    wdt_thread.start(wdt_fn);

    // Set the UART comms speed
    pc.baud(230400);
    pc2.baud(230400);

    ThisThread::sleep_for(1000);

    debug_printf(DBG_INFO, "Starting serial threads...\r\n"); // Removing this causes a hard fault???

    // Start the serial handler threads
    tx_serial_thread.start(tx_serial_thread_fn);
    rx_serial_thread.start(rx_serial_thread_fn);
    rx_frame_thread.start(rx_frame_ser_thread_fn);
    debug_printf(DBG_INFO, "Serial threads started\r\n");
    send_status();

    // Mount the filesystem, load the configuration, log the bootup
    init_filesystem();
    load_settings_from_flash();
    save_settings_to_flash();
    log_boot();

    // Wait for 2 seconds in MANAGEMENT mode
    current_mode = MANAGEMENT;
    send_status();
    led1.LEDFastBlink();
    ThisThread::sleep_for(2000);
    while(stay_in_management) {
        ThisThread::sleep_for(5000);
    }
    current_mode = RUNNING;
    send_status();

    led1.LEDBlink();
    led2.LEDOff();
    led3.LEDOff();

#if 0
    // Test the FEC
    debug_printf(DBG_INFO, "Now testing the FEC\r\n");
    auto fec_frame = make_shared<Frame>();  
    debug_printf(DBG_INFO, "Size of fec_frame is %d\r\n", fec_frame->getPktSize());

    print_memory_info();

    {
    auto fec_test_fec = make_shared<FEC>();
    fec_test_fec->benchmark(25);
    auto fec_test_interleave = make_shared<FECInterleave>();
    fec_test_interleave->benchmark(25);
    auto fec_test_conv = make_shared<FECConv>(2, 9);
    fec_test_conv->benchmark(25);
    auto fec_test_rsv = make_shared<FECRSV>(2, 9, 8);
    fec_test_rsv->benchmark(25);
    } 
#endif

    // Start the NVRAM logging thread
    debug_printf(DBG_INFO, "Starting the NV logger\r\n");
    nv_log_thread.start(nv_log_fn);

    ThisThread::sleep_for(250);

    // Set up the radio
    debug_printf(DBG_INFO, "Initializing the Radio\r\n");
    init_radio();
    ThisThread::sleep_for(250);

    // Start the mesh protocol thread
    debug_printf(DBG_INFO, "Starting the mesh protocol thread\r\n");
    mesh_protocol_thread.start(mesh_protocol_fsm);

    
    debug_printf(DBG_INFO, "Time to chill...\r\n");

    ThisThread::sleep_for(250);

    // Start the beacon thread
    debug_printf(DBG_INFO, "Starting the beacon thread\r\n");
    beacon_thread.start(beacon_fn);

    ThisThread::sleep_for(250);

    debug_printf(DBG_INFO, "Started all threads\r\n");

}

