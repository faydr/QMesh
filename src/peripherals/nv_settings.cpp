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
#include "LittleFileSystem.h"
#include "serial_data.hpp"
#include "nv_settings.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "MbedJSONValue.h"

DigitalOut flash_io2(MBED_CONF_APP_FLASH_IO2);
DigitalOut flash_io3(MBED_CONF_APP_FLASH_IO3);
SPIFBlockDevice bd(MBED_CONF_APP_FLASH_SPI_MOSI, 
                    MBED_CONF_APP_FLASH_SPI_MISO, 
                    MBED_CONF_APP_FLASH_SPI_SCLK, 
                    MBED_CONF_APP_FLASH_SPI_CS);
LittleFileSystem fs("fs");


void rescue_filesystem(void) {
    flash_io2 = 1;
    flash_io3 = 1;
    int err = bd.init();
	err = fs.reformat(&bd);
}

void init_filesystem(void) {
    flash_io2 = 1;
    flash_io3 = 1;
    debug_printf(DBG_INFO, "Now mounting the block device\r\n");
    int err = bd.init();
    debug_printf(DBG_INFO, "bd.init -> %d  \r\n", err);
    debug_printf(DBG_INFO, "bd size: %llu\n",         bd.size());
    debug_printf(DBG_INFO, "bd read size: %llu\n",    bd.get_read_size());
    debug_printf(DBG_INFO, "bd program size: %llu\n", bd.get_program_size());
    debug_printf(DBG_INFO, "bd erase size: %llu\n",   bd.get_erase_size());
    debug_printf(DBG_INFO, "Now mounting the filesystem...\r\n");
    err = fs.mount(&bd);
    debug_printf(DBG_WARN, "%s\r\n", (err ? "Fail :(" : "OK"));
    if(err) {
        debug_printf(DBG_WARN, "No filesystem found, reformatting...\r\n");
        err = fs.reformat(&bd);
        debug_printf(DBG_WARN, "%s\r\n", (err ? "Fail :(" : "OK"));
        MBED_ASSERT(!err);
        int err = fs.mount(&bd);
        MBED_ASSERT(!err);
    }
	// Display the root directory
    debug_printf(DBG_INFO, "Opening the root directory... \r\n");
    fflush(stdout);
    DIR *d = opendir("/fs/");
    debug_printf(DBG_INFO, "%s\n", (!d ? "Fail :(\r\n" : "OK\r\n"));
    if (!d) {
        error("error: %s (%d)\n", strerror(errno), -errno);
    }
	for(;;) {
		struct dirent *dir_val = readdir(d);
		if(dir_val == NULL) {
			break;
		}
		struct stat file_stat;
		fs.stat(dir_val->d_name, &file_stat);
		debug_printf(DBG_INFO, "%s, Size: %d\r\n", dir_val->d_name, file_stat.st_size);
	}
}

void load_settings_from_flash(void) {
    debug_printf(DBG_INFO, "Stats on settings.json\r\n");
    FILE *f;
    f = fopen("/fs/settings.json", "r");
    if(!f) {
        debug_printf(DBG_WARN, "Unable to open settings.json. Creating new file with default settings\r\n");
        f = fopen("/fs/settings.json", "w");
        radio_cb["Mode"] = "Mesh Normal";
        radio_cb["Frequency"] = RADIO_FREQUENCY;
        radio_cb["BW"] = RADIO_BANDWIDTH;
        radio_cb["CR"] = RADIO_CODERATE;
        radio_cb["SF"] = RADIO_SF;
        radio_cb["Preamble Length"] = RADIO_PREAMBLE_LEN;
        radio_cb["Preamble Slots"] = RADIO_PREAMBLE_SLOTS;
        radio_cb["Beacon Message"] = RADIO_BEACON_MSG;
        radio_cb["Beacon Interval"] = RADIO_BEACON_INTERVAL;
        radio_cb["Payload Length"] = FRAME_PAYLOAD_LEN;
        radio_cb["FEC Algorithm"] = FEC_ALGORITHM;
        radio_cb["Convolutional Rate"] = FEC_CONV_RATE;
        radio_cb["Convolutional Order"] = FEC_CONV_ORDER;
        radio_cb["Reed-Solomon Number Roots"] = FEC_RS_NUM_ROOTS;
	    radio_cb["TX Power"] = RADIO_POWER;
        radio_cb["CW Test Mode"] = 0;
        radio_cb["Preamble Test Mode"] = 0;
        radio_cb["Test FEC"] = 0;
        string settings_str = radio_cb.serialize();
        fprintf(f, "%s\r\n", settings_str.c_str());
        fflush(f);
        fclose(f);
        f = fopen("/fs/settings.json", "r");
        MBED_ASSERT(f);
    }
    struct stat file_stat;
    stat("/fs/settings.json", &file_stat);
    debug_printf(DBG_INFO, "Size is %d\r\n", file_stat.st_size);
	vector<char> linebuf(file_stat.st_size);
	fread(linebuf.data(), 1, file_stat.st_size, f);
	debug_printf(DBG_INFO, "Read from file: %s\r\n", linebuf.data());
    parse(radio_cb, linebuf.data());
    fflush(f);
    fclose(f);
    debug_printf(DBG_INFO, "Mode: %s\r\n", radio_cb["Mode"].get<string>().c_str());
    debug_printf(DBG_INFO, "Frequency: %d\r\n", radio_cb["Frequency"].get<int>());
    debug_printf(DBG_INFO, "BW: %d\r\n", radio_cb["BW"].get<int>());
    debug_printf(DBG_INFO, "CR: %d\r\n", radio_cb["CR"].get<int>());
    debug_printf(DBG_INFO, "SF: %d\r\n", radio_cb["SF"].get<int>());
    debug_printf(DBG_INFO, "Preamble Length: %d\r\n", radio_cb["Preamble Length"].get<int>());
    debug_printf(DBG_INFO, "Preamble Slots: %d\r\n", radio_cb["Preamble Slots"].get<int>());
    debug_printf(DBG_INFO, "Beacon Message: %s\r\n", radio_cb["Beacon Message"].get<string>().c_str());
    debug_printf(DBG_INFO, "Beacon Interval: %d\r\n", radio_cb["Beacon Interval"].get<int>());
    debug_printf(DBG_INFO, "Payload Length: %d\r\n", radio_cb["Payload Length"].get<int>());
}

void save_settings_to_flash(void) {
    debug_printf(DBG_INFO, "Opening settings.json...\r\n");
    FILE *f = fopen("/fs/settings.json", "w"); 
    MBED_ASSERT(f != 0);
    string settings_str = radio_cb.serialize();
    fwrite(settings_str.c_str(), 1, settings_str.size(), f);
    fclose(f);
}

void log_boot(void) {
    FILE *f = fopen("/fs/boot_log.json", "a+");
    MBED_ASSERT(f);
    MbedJSONValue log_json;
    log_json["Unix Time"] = (int) time(NULL);
    time_t my_time = time(NULL);
    char *time_str = ctime(&my_time);
    log_json["Time String"] = string(time_str);
    string log_json_str = log_json.serialize();
	debug_printf(DBG_INFO, "Wrote %s\r\n", log_json_str.c_str());
    fwrite(log_json_str.c_str(), 1, log_json_str.size(), f);
    fflush(f);
	fclose(f);
}

void nv_log_fn(void) {

    for(;;) {
		FILE *f = fopen("/fs/logfile.json", "a+");
		MBED_ASSERT(f);
        // Write the latest frame to disk
        auto log_frame = dequeue_mail(nv_logger_mail);  
        MbedJSONValue log_json;
        int16_t rssi;
        uint16_t rx_size;
        int8_t snr;
        log_frame->getRxStats(rssi, snr, rx_size);
		time_t my_time = time(NULL);
		log_json["Timestamp"] = ctime(&my_time);
        log_json["RSSI"] = (int) rssi;
        log_json["SNR"] = (int) snr;
        log_json["RX Size"] = (int) rx_size;
        log_json["Computed CRC"] = log_frame->calcCRC();
        log_json["CRC"] = log_frame->getCRC();
        string log_json_str = log_json.serialize();
        fwrite(log_json_str.c_str(), 1, log_json_str.size(), f);
        fflush(f);
		fclose(f);
        // Check whether we've filled up the SPI flash chip. If so,
        //  delete the file and reopen it as an empty one.
        struct stat st;
        stat("/fs/logfile.json", &st);
        if(st.st_size > 12000000) {
            debug_printf(DBG_INFO, "Log file is >12MB. Deleting and reopening...\r\n");
            fclose(f);
            fopen("/fs/logfile.json", "w+");
            MBED_ASSERT(f);
        }
    }
}
