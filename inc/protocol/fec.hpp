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

#ifndef FEC_HPP
#define FEC_HPP

#include "mbed.h"
#include "correct.h"
#include "params.hpp"
#include <cmath>
#include <map>
#include "PolarCode.h"
#include <string>


// Convolutional Codes

// Polynomials
// These have been determined via find_conv_libfec_poly.c
// We could just make up new ones, but we use libfec's here so that
//   codes encoded by this library can be decoded by the original libfec
//   and vice-versa
#define V27POLYA 0155
#define V27POLYB 0117
static correct_convolutional_polynomial_t libfec_r12_7_polynomial[] = {V27POLYA, V27POLYB};

#define V29POLYA 0657
#define V29POLYB 0435
static correct_convolutional_polynomial_t libfec_r12_9_polynomial[] = {V29POLYA, V29POLYB};

#define V39POLYA 0755
#define V39POLYB 0633
#define V39POLYC 0447
static correct_convolutional_polynomial_t libfec_r13_9_polynomial[] = {V39POLYA, V39POLYB, V39POLYC};

#define V615POLYA 042631
#define V615POLYB 047245
#define V615POLYC 056507
#define V615POLYD 073363
#define V615POLYE 077267
#define V615POLYF 064537
static correct_convolutional_polynomial_t libfec_r16_15_polynomial[] = {V615POLYA, V615POLYB, V615POLYC,
                        V615POLYD, V615POLYE, V615POLYF};

static correct_convolutional_polynomial_t conv_r12_6_polynomial[] = {073, 061};
static correct_convolutional_polynomial_t conv_r12_7_polynomial[] = {0161, 0127};
static correct_convolutional_polynomial_t conv_r12_8_polynomial[] = {0225, 0373};
static correct_convolutional_polynomial_t conv_r12_9_polynomial[] = {0767, 0545};
static correct_convolutional_polynomial_t conv_r13_6_polynomial[] = {053, 075, 047};
static correct_convolutional_polynomial_t conv_r13_7_polynomial[] = {0137, 0153,
                                                                                   0121};
static correct_convolutional_polynomial_t conv_r13_8_polynomial[] = {0333, 0257,
                                                                                   0351};
static correct_convolutional_polynomial_t conv_r13_9_polynomial[] = {0417, 0627,
                                                                                   0675};                        


// This class provides a way to magically apply forward error correction to 
//  a character array provided.
class FEC {
protected:
    string name;

public:
    FEC(void) {
        name = "Dummy FEC";
    }

    static float getBER(const float snr);

    static bool getBit(const vector<uint8_t> &bytes, const size_t pos) {
        uint8_t byte = bytes[pos/8];
        uint8_t byte_pos = pos % 8;
        return ((1 << byte_pos) & byte) == 0 ? false : true;
    }

    static void setBit(const bool bit, const size_t pos, vector<uint8_t> &bytes) {
        bytes[pos/8] &= ~(1 << pos);
        uint8_t my_bit = bit == false ? 0 : 1;
        bytes[pos/8] |= (my_bit << pos);
    }

    static void interleaveBits(const vector<uint8_t> &bytes, vector<uint8_t> &int_bytes) {
        int_bytes.resize(bytes.size());
        size_t non_int_pos = 0;
        for(size_t cur_bit = 0; cur_bit < 8; cur_bit++) {
            for(size_t cur_byte = 0; cur_byte < bytes.size(); cur_byte++) {
                bool bit = getBit(bytes, non_int_pos);
                setBit(bit, cur_byte*8+cur_bit, int_bytes);
                non_int_pos += 1;
            }
        }
    }

    static void deinterleaveBits(const vector<uint8_t> &int_bytes, vector<uint8_t> &bytes) {
        bytes.resize(int_bytes.size());
        size_t non_int_pos = 0;
        for(size_t cur_bit = 0; cur_bit < 8; cur_bit++) {
            for(size_t cur_byte = 0; cur_byte < bytes.size(); cur_byte++) {
                bool bit = getBit(int_bytes, cur_byte*8+cur_bit);
                setBit(bit, non_int_pos, bytes);
                non_int_pos += 1;
            }
        }
    }

    virtual size_t getEncSize(const size_t msg_len) {
        return msg_len;
    }

    virtual size_t encode(const vector<uint8_t> &msg, vector<uint8_t> &enc_msg) {
        enc_msg = msg;
        return msg.size();
    }

    virtual ssize_t decode(const vector<uint8_t> &enc_msg, vector<uint8_t> &dec_msg) {
        dec_msg = enc_msg;
        return dec_msg.size();
    }
 
    virtual ssize_t decodeSoft(const vector<uint8_t> &enc_msg, vector<uint8_t> &dec_msg, const float snr) {
        return decode(enc_msg, dec_msg);
    }

    void benchmark(size_t num_iters);
};


class FECInterleave : public FEC {
public:
    FECInterleave(void) {
        name = "Dummy Interleaver";   
    }

    size_t encode(const vector<uint8_t> &msg, vector<uint8_t> &enc_msg) {
        FEC::interleaveBits(msg, enc_msg);
        return enc_msg.size();
    }

    ssize_t decode(const vector<uint8_t> &enc_msg, vector<uint8_t> &dec_msg) {
        FEC::deinterleaveBits(enc_msg, dec_msg);
        return dec_msg.size();
    }  
};


class FECPolar : public FEC {
protected:
    shared_ptr<PolarCodeLib::PolarCode> polar_code;
    size_t block_length;
    size_t info_length;
    size_t list_size;
public:
    FECPolar(const size_t my_block_length, const size_t my_info_length, const size_t my_list_size) {
        name = "Polar Codes";
        block_length = my_block_length;
        info_length = my_info_length;
        list_size = my_list_size;
        polar_code = make_shared<PolarCodeLib::PolarCode>(PolarCodeLib::PolarCode(block_length, info_length, 0.32, 2));
    }

    size_t getEncSize(const size_t msg_len) {
        return (1 << block_length);
    }

    size_t encode(const vector<uint8_t> &msg, vector<uint8_t> &enc_msg) {
        //std::vector<uint8_t> encode(std::vector<uint8_t> info_bits);
        enc_msg = polar_code->encode(msg);
        return enc_msg.size();
    }

    ssize_t decode(const vector<uint8_t> &enc_msg, vector<uint8_t> &dec_msg) {
        vector<float> p0, p1;
        for(vector<uint8_t>::const_iterator iter = enc_msg.begin(); iter != enc_msg.end(); iter++) {
            uint8_t my_byte = *iter;
            for(int i = 0; i < 8; i++) {
                if(my_byte & 0x1) {
                    p0.push_back(0.0);
                    p1.push_back(1.0);
                }
                else {
                    p0.push_back(1.0);
                    p1.push_back(0.0);
                }
                my_byte >>= 1;
            }
        }
        //int list_size = radio_cb["Polar List Size"].get<int>();
        dec_msg = polar_code->decode_scl_p1(p1, p0, list_size);
        return dec_msg.size();
    }

    ssize_t decodeSoft(const vector<uint8_t> &enc_msg, vector<uint8_t> &dec_msg, const float snr) {
        vector<float> p0, p1;
        for(vector<uint8_t>::const_iterator iter = enc_msg.begin(); iter != enc_msg.end(); iter++) {
            uint8_t my_byte = *iter;
            for(int i = 0; i < 8; i++) {
                if(my_byte & 0x1) {
                    p0.push_back(0.0);
                    p1.push_back(1.0);
                }
                else {
                    p0.push_back(1.0);
                    p1.push_back(0.0);
                }
                my_byte >>= 1;
            }
        }
        dec_msg = polar_code->decode_scl_p1(p1, p0, list_size);
        return dec_msg.size();
    }
};


class FECConv: public FEC {
protected:
    // Convolutional coding parameters
    size_t inv_rate;
    size_t order;
    correct_convolutional *corr_con;    

public:    
    FECConv(void) : FECConv(2, 9) { }

    FECConv(const size_t inv_rate, const size_t order);

    ~FECConv(void) {
        correct_convolutional_destroy(corr_con);
    }

    size_t getEncSize(const size_t msg_len) {
        return correct_convolutional_encode_len(corr_con, msg_len)/8;
    }

    size_t encode(const vector<uint8_t> &msg, vector<uint8_t> &enc_msg) {
        enc_msg.resize(getEncSize(msg.size()));
        vector<uint8_t> msg_int(msg.size());
        interleaveBits(msg, msg_int);
        return correct_convolutional_encode(corr_con, msg_int.data(), msg_int.size(), enc_msg.data())/8;
    }

    ssize_t decode(const vector<uint8_t> &enc_msg, vector<uint8_t> &dec_msg) {
        dec_msg.resize(enc_msg.size());
        vector<uint8_t> dec_msg_int(dec_msg.size());
        size_t dec_size = correct_convolutional_decode(corr_con, enc_msg.data(), 
                                enc_msg.size()*8, dec_msg_int.data());
        dec_msg.resize(dec_size);
        deinterleaveBits(dec_msg_int, dec_msg);
        return dec_size;
    }

};


class FECRSV: public FECConv {
protected:
    size_t rs_corr_bytes;
    correct_reed_solomon *rs_con;
    vector<uint8_t> rs_buf;

public:
    FECRSV(const size_t inv_rate, const size_t order, const size_t my_rs_corr_bytes) 
        : FECConv(inv_rate, order) {
        name = "RSV";
        rs_corr_bytes = my_rs_corr_bytes;
        rs_con = correct_reed_solomon_create(correct_rs_primitive_polynomial_ccsds,
                                                  1, 1, rs_corr_bytes);
    }

    FECRSV(void) : FECRSV(2, 9, 32) { };

    ~FECRSV(void ) {
        correct_reed_solomon_destroy(rs_con);
    }

    size_t encode(const vector<uint8_t> &msg, vector<uint8_t> &rsv_enc_msg) {
        MBED_ASSERT(getEncSize(msg.size()) <= 256);
        vector<uint8_t> rs_enc_msg(msg.size()+rs_corr_bytes);
        size_t rs_size = correct_reed_solomon_encode(rs_con, msg.data(), msg.size(), rs_enc_msg.data());
        MBED_ASSERT(rs_enc_msg.size() == rs_size);
        rsv_enc_msg.resize(getEncSize(msg.size()));
        size_t conv_len = FECConv::encode(rs_enc_msg, rsv_enc_msg);
        MBED_ASSERT(rsv_enc_msg.size() == conv_len);
        return conv_len;
    }

    ssize_t decode(const vector<uint8_t> &rsv_enc_msg, vector<uint8_t> &dec_msg) {
        vector<uint8_t> rs_enc_msg(rsv_enc_msg.size());
        size_t conv_bytes = FECConv::decode(rsv_enc_msg, rs_enc_msg);
        MBED_ASSERT(conv_bytes != -1);
        rs_enc_msg.resize(conv_bytes);
        dec_msg.resize(rs_enc_msg.size()-rs_corr_bytes);
        size_t rs_len = correct_reed_solomon_decode(rs_con, rs_enc_msg.data(), rs_enc_msg.size(), 
                            dec_msg.data());
        MBED_ASSERT(dec_msg.size() == rs_len);
        return dec_msg.size();
    }

    size_t getEncSize(const size_t msg_len) {
        size_t enc_size = FECConv::getEncSize(msg_len+rs_corr_bytes);
        MBED_ASSERT(enc_size <= 256);
        return enc_size;
    }
};

#endif /* FEC_HPP */