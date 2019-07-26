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
#ifndef RADIO_HPP
#define RADIO_HPP

#include "mbed.h"
#include <stdint.h>
#include <memory>
#include <vector>

// Radio settings struct. 
// Why? Because it makes it easy to save and restore the state to NVRAM
typedef struct {
    uint32_t bw;
    uint32_t sf;
    uint8_t cr;
    uint32_t freq;
} nv_radio_settings_t;


union {
    nv_radio_settings_t radio_settings;
    uint8_t buf[sizeof(nv_radio_settings_t)];
} nv_radio_settings_union;

// Main thread for working with the LoRa radio
extern Thread radio_thread;

// Initialize the radio
void init_radio(void);

// Test functions, one for transmitting and one for receiving
#define RX_DONE_SIGNAL 0xAB
void tx_test_radio(void);
void rx_test_radio(void);

/*!
 * Frequency hopping frequencies table
 */
const int32_t hopping_channels[] =
{
      3, // 916500000,
     17, // 923500000,
    -17, // 906500000,
      5, // 917500000,
      5, // 917500000,
    -12, // 909000000,
    -24, // 903000000,
      2, // 916000000,
     -5, // 912500000,
     22, // 926000000,
     20, // 925000000,
    -11, // 909500000,
     -4, // 913000000,
      7, // 918500000,
      7, // 918500000,
    -25, // 902500000,
     -7, // 911500000,
     23, // 926500000,
    -25, // 902500000,
     14, // 922000000,
     18, // 924000000,
    -23, // 903500000,
     -4, // 913000000,
     14, // 922000000,
     22, // 926000000,
    -10, // 910000000,
     10, // 920000000,
     15, // 922500000,
     -8, // 911000000,
     14, // 922000000,
    -11, // 909500000,
     22, // 926000000,
     14, // 922000000,
      6, // 918000000,
     21, // 925500000,
    -14, // 908000000,
      5, // 917500000,
     23, // 926500000,
    -13, // 908500000,
      2, // 916000000,
    -19, // 905500000,
      2, // 916000000,
    -24, // 903000000,
    -10, // 905000000,
     -4, // 913000000,
    -16, // 907000000,
    -10, // 910000000,
     12, // 926500000,
     21, // 925500000,
     -8, // 911000000
};


typedef enum {
    TX_DONE_EVT,
    RX_DONE_EVT,
    TX_TIMEOUT_EVT,
    RX_TIMEOUT_EVT,
    RX_ERROR_EVT,
} radio_evt_enum_t;

class RadioEvent {
public:
    radio_evt_enum_t evt_enum;
    Timer timer;
    int16_t rssi;
    int8_t snr;
    std::shared_ptr<vector<uint8_t>> buf;

    RadioEvent(const radio_evt_enum_t my_evt_enum);

    RadioEvent(const radio_evt_enum_t my_evt_enum, const uint8_t *my_buf, 
                const size_t my_size, const int16_t my_rssi, const int8_t my_snr);
};

extern Mail<shared_ptr<RadioEvent>, 16> radio_evt_mail;


#endif /* RADIO_HPP */