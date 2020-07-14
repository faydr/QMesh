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
#include "TinyGPSPlus.h"
#include "LibAPRS.h"
#include "AFSK.h"

//UARTSerial gps_serial(MBED_CONF_APP_GPS_UART_TX, MBED_CONF_APP_GPS_UART_RX, 9600);
//TinyGPSPlus gps;
GNSS *gnss;
JSONSerial rx_json_ser, tx_json_ser;
Thread tx_serial_thread(osPriorityNormal, 8192, NULL, "TX-SERIAL");
Thread rx_serial_thread(osPriorityNormal, 8192, NULL, "RX-SERIAL");
Thread mesh_protocol_thread(osPriorityNormal, 4096, NULL, "MESH-FSM");
Thread rx_frame_thread(osPriorityNormal, 4096, NULL, "RX-FRAME");
Thread beacon_thread(osPriorityNormal, 4096, NULL, "BEACON");
Thread nv_log_thread(osPriorityNormal, 4096, NULL, "NV-LOG");
Thread button_thread(osPriorityNormal, 4096, NULL, "BUTTON");
Thread oled_mon_thread(osPriorityNormal, 4096, NULL, "OLED-MON");
Thread btn_evt_thread(osPriorityNormal, 4096, NULL, "BTN-EVT");
Thread gps_thread(osPriorityNormal, 4096, NULL, "GPSD");

Afsk my_afsk;

system_state_t current_mode = BOOTING;
bool stay_in_management = false;

#define SLEEP_TIME                  500 // (msec)
#define PRINT_AFTER_N_LOOPS         20

void send_status(void);

DigitalIn user_button(USER_BUTTON);

SoftI2C oled_i2c(PB_8, PB_9);
Adafruit_SSD1306_I2c *oled;

void print_stats()
{
    {
    mbed_stats_cpu_t stats;
    mbed_stats_cpu_get(&stats);

    printf("Uptime: %-20lld", stats.uptime);
    printf("Idle time: %-20lld", stats.idle_time);
    printf("Sleep time: %-20lld", stats.sleep_time);
    printf("Deep sleep time: %-20lld\n", stats.deep_sleep_time);
    }
#if 0
    {
    mbed_stats_thread_t *stats = new mbed_stats_thread_t[20];
    int count = mbed_stats_thread_get_each(stats, 20);
    
    for(int i = 0; i < count; i++) {
        printf("ID: 0x%x \n", stats[i].id);
        printf("Name: %s \n", stats[i].name);
        printf("State: %d \n", stats[i].state);
        printf("Priority: %d \n", stats[i].priority);
        printf("Stack Size: %d \n", stats[i].stack_size);
        printf("Stack Space: %d \n", stats[i].stack_space);
        printf("\n");
    }
    }
#endif
}


// main() runs in its own thread in the OS

static int dummy = printf("Starting all the things\r\n");
int main()
{
    oled_i2c.frequency(400000);
    oled_i2c.start();
    
    oled = new Adafruit_SSD1306_I2c(oled_i2c, PD_13, 0x78, 32, 128);

    oled->printf("Welcome to QMesh\r\n");
    oled->display();

    led1.LEDBlink();
    oled->printf("In rescue mode...\r\n");
    oled->display();
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
	
    // Start the WDT thread
    wdt_thread.start(wdt_fn);
    ThisThread::sleep_for(1000);

    debug_printf(DBG_INFO, "Starting serial threads...\r\n"); // Removing this causes a hard fault???
    // Start the serial handler threads
    tx_serial_thread.start(tx_serial_thread_fn);
    rx_frame_thread.start(rx_frame_ser_thread_fn);
    debug_printf(DBG_INFO, "Serial threads started\r\n");
    send_status();
    printf("Hello\r\n");

    // Start up the GPS code
    debug_printf(DBG_INFO, "Starting the GPS...\r\n");
    gnss = new GNSS(MBED_CONF_APP_GPS_UART_TX, MBED_CONF_APP_GPS_UART_RX);
    gnss->init();
#if 1
    float lon, lat, acc;
    fixType_t fix;
    gnss->getCoodinates(lon, lat, fix, acc);
    debug_printf(DBG_INFO, "lat %f, lon %f\r\n", lat, lon);
#endif

    // Set up the RDA1846 module control
    DRA818(PD_5, PD_6, PD_7, PD_4, PD_3, PE_2);

    // Initialize the LibAPRS components
    debug_printf(DBG_INFO, "Starting LibAPRS...\r\n");
    AFSK_init(&my_afsk);
    APRS_init(0, false);
    string call_str = "NOCALL";
    APRS_setCallsign((char *) call_str.data(), 1);
    string lat_str = "5530.80N";
    APRS_setLat((char *) lat_str.c_str());
    string lon_str = "01143.89E";
    APRS_setLon((char *) lon_str.c_str());
    APRS_setPower(2);
    APRS_setHeight(4);
    APRS_setGain(7);
    APRS_setDirectivity(0);
    APRS_printSettings();
    // Try to send out a test packet
    string comment = "LibAPRS location update";
    //while(1) {
        APRS_sendLoc((char *) comment.c_str(), strlen(comment.c_str()));
    //}

    // Mount the filesystem, load the configuration, log the bootup
    init_filesystem();
    load_settings_from_flash();
    save_settings_to_flash();
    log_boot();

    // Wait for 2 seconds in MANAGEMENT mode
    current_mode = MANAGEMENT;
    oled->printf("MANAGEMENT mode...\r\n");
    oled->display();
    oled->display();
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

    // Test the FEC
#if 0
    debug_printf(DBG_INFO, "Now testing the FEC\r\n");
    auto fec_frame = make_shared<Frame>();  
    debug_printf(DBG_INFO, "Size of fec_frame is %d\r\n", fec_frame->codedSize());
    print_memory_info();
    {
    auto fec_test_fec = make_shared<FEC>(Frame::size());
    fec_test_fec->benchmark(100);
    auto fec_test_interleave = make_shared<FECInterleave>(Frame::size());
    fec_test_interleave->benchmark(100);
    auto fec_test_conv = make_shared<FECConv>(Frame::size(), 2, 9);
    fec_test_conv->benchmark(100);
    ThisThread::sleep_for(500);
    auto fec_test_rsv = make_shared<FECRSV>(Frame::size(), 2, 9, 8);
    fec_test_rsv->benchmark(100);
    ThisThread::sleep_for(500);
    print_memory_info();
    auto fec_test_rsv_big = make_shared<FECRSV>(Frame::size(), 3, 9, 8);
    fec_test_rsv_big->benchmark(100);
    ThisThread::sleep_for(500);
    print_memory_info();
    } 
print_memory_info();
ThisThread::sleep_for(500);
while(1);
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
 
    // Start the OLED monitoring thread
    oled_mon_thread.start(oled_mon_fn);

    debug_printf(DBG_INFO, "Started all threads\r\n");

    for(;;) {
        print_stats();
        ThisThread::sleep_for(5000);
    }
}

