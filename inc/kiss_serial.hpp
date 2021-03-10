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

#ifndef JSON_SERIAL_HPP
#define JSON_SERIAL_HPP

/* 
    This code exists to allow data to go across the serial port as JSON-formatted
    data. Doing so allows for different types of data (frames, debug messages, 
    configuration commands) to use the same UART simultaneously, while keeping
    all data being conveyed as at least somewhat-readable using just a serial 
    terminal application.

    To facilitate readability within a terminal application, any binary data is 
    encoded as Base64.
*/

#include "mbed.h"
#include "peripherals.hpp"
#include "serial_data.hpp"
#include <string>
#include "Adafruit_SSD1306.h"
#include "mesh_protocol.hpp"
#include "qmesh.pb.h"
#include "pb_common.h"
#include "pb_encode.h"
#include "pb_decode.h"

typedef uint16_t crc_t;
typedef uint16_t entry_size_t;
typedef enum {
    READ_SUCCESS = 0,
    READ_SER_EOF,
    READ_INVALID_KISS_ID,
    READ_ENTRY_SIZE_ERR,
    READ_MSG_OVERRUN_ERR,
    INVALID_ENTRY_SIZE,
    READ_SER_MSG_ERR,
    DECODE_SER_MSG_ERR,
    READ_CRC_ERR,
    CRC_ERR,
    INVALID_CHAR
} read_ser_msg_err_t;

typedef enum {
    WRITE_SUCCESS = 0,
    ENCODE_SER_MSG_ERR, 
    WRITE_SER_MSG_ERR
} write_ser_msg_err_t;

#if 0
static const uint8_t FEND = 0xC0;
static const uint8_t FESC = 0xDB;
static const uint8_t TFEND = 0xDC;
static const uint8_t TFESC = 0xDD;
static const uint8_t SETHW = 0x06;
static const uint8_t DATAPKT = 0x00;
static const size_t MAX_MSG_SIZE = (SerialMsg_size+sizeof(crc_t))*2;
#endif

write_ser_msg_err_t save_SerialMsg(const SerialMsg &ser_msg, FILE *f, const bool kiss_data_msg = false);
read_ser_msg_err_t load_SerialMsg(SerialMsg &ser_msg, FILE *f);

extern Mail<shared_ptr<SerialMsg>, QUEUE_DEPTH> tx_ser_queue;

/// Produces an MbedJSONValue with the current status and queues it for transmission.
void tx_serial_thread_fn(void);
/// Serial thread function that receives serial data, and processes it accordingly.
void rx_serial_thread_fn(void);

#endif /* JSON_SERIAL_HPP */