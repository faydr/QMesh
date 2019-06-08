/*
 * Copyright (c) 2019, Daniel R. Fay.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "lora_radio_helper.h"
#include "radio.hpp"
#include "params.hpp"
#include "serial_data.hpp"
#include "nv_settings.hpp"
#include "fec.hpp"


I2C i2c(PB_9, PB_8);
EEPROM eeprom(&i2c);
NVSettings *nv_settings;
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
Thread led2_thread;
Thread led3_thread;

void led2_thread_fn() {
    while (true) {
        led2 = !led2;
        wait(0.333);
    }
}

void led3_thread_fn() {
    while (true) {
        led3 = !led3;
        wait(0.1);
    }
}

#define SLEEP_TIME                  500 // (msec)
#define PRINT_AFTER_N_LOOPS         20

// main() runs in its own thread in the OS
Serial pc(USBTX, USBRX);
int main()
{
    // Set the UART comms speed
    pc.baud(230400);

    // Set up and test the EEPROM
#ifdef TEST_EEPROM
    debug_printf(DBG_INFO, "Now testing the EEPROM\r\n");
    eeprom.testEEPROM();
#endif

    nv_settings = new NVSettings(&eeprom);

    // Test the FEC
    debug_printf(DBG_INFO, "Now testing the FEC\r\n");
    Frame *fec_frame = new Frame();  
    FEC *fec;  
#ifdef FEC_CONV
    fec = new FECRSV(fec_frame->getPktSize(), 2, 7);
#elif defined FEC_RSV
    fec = new FECConv(fec_frame->getPktSize(), 2, 7);
#else
#error "Need to define either FEC_CONV or FEC_RSV\r\n"
#endif
    fec->benchmark(1000);
    delete fec_frame;
    static uint8_t fec_enc_buf[512];
    static uint8_t fec_dec_buf[512];
    static char test_msg[] = "0123456789\r\n";
    debug_printf(DBG_INFO, "Encoding %s", test_msg);
    fec->encode((uint8_t *) test_msg, 13, fec_enc_buf);
    fec->decode(fec_enc_buf, 26, fec_dec_buf);
    debug_printf(DBG_INFO, "Decoded %s", fec_dec_buf);

    // Set up the radio
    debug_printf(DBG_INFO, "Initializing radio\r\n");
    init_radio();

    // Start a thread for blinking LEDs
    debug_printf(DBG_INFO, "Starting LED2 thread\r\n");
    led2_thread.start(led2_thread_fn);
    debug_printf(DBG_INFO, "Starting LED3 thread\r\n");
    led3_thread.start(led3_thread_fn);

#ifdef TX_TEST_MODE
    // Start a thread for the radio
    debug_printf(DBG_INFO, "Starting Tx test thread\r\n");
    radio_thread.start(tx_test_radio);
#elif defined RX_TEST_MODE
    // Start a thread for the radio_thread
    debug_printf(DBG_INFO, "Starting Rx test thread\r\n");
    radio_thread.start(rx_test_radio);
#elif defined MESH_TEST_MODE_ORIGINATOR
    // Start a thread for the radio_thread
    debug_printf(DBG_INFO, "Starting Mesh test originator thread\r\n");
    radio_thread.start(mesh_originator_test);
#else
    debug_printf(DBG_INFO, "Starting the mesh network\r\n");
#endif

    int count = 0;
    while (true) {
        // Blink LED and wait 0.5 seconds
        led1 = !led1;
        wait_ms(SLEEP_TIME);
    }
}

