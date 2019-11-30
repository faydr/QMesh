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
static correct_convolutional_polynomial_t conv_r13_7_polynomial[] = {0137, 0153, 0121};
static correct_convolutional_polynomial_t conv_r13_8_polynomial[] = {0333, 0257, 0351};
static correct_convolutional_polynomial_t conv_r13_9_polynomial[] = {0417, 0627, 0675};                        

/**
 * Base class for Forward Error Correction. Provides some generically-useful functions,
 * like interleaving, but otherwise just functions as a dummy FEC class.
 */
class FEC {
protected:
    string name;

public:
    /// Constructor.
    FEC(void) {
        name = "Dummy FEC";
    }

    /**
     * Takes the LoRa packet SNR and returns a corresponding a bit error rate.
     * @param snr The LoRa SNR.
     */
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

    /**
     * Get the encoded size.
     * @param msg_len The unencoded size of the message.
     */
    virtual size_t getEncSize(const size_t msg_len) {
        return msg_len;
    }

    /**
     * Apply the FEC coding. Returns the encoded size, in bytes.
     * @param msg Byte vector of data to be encoded.
     * @param enc_msg Byte vector of encoded data.
     */
    virtual size_t encode(const vector<uint8_t> &msg, vector<uint8_t> &enc_msg) {
        enc_msg = msg;
        return msg.size();
    }

    /**
     * Decode FEC-coded data. Returns the decoded data size, in bytes.
     * @param enc_msg Byte vector of encoded data.
     * @param dec_msg Byte vector of decoded data.
     */
    virtual ssize_t decode(const vector<uint8_t> &enc_msg, vector<uint8_t> &dec_msg) {
        dec_msg = enc_msg;
        return dec_msg.size();
    }
 
    /**
     * Basic soft decoding. This soft decoding uses the whole-packet SNR to get a BER and thus
     * fake some sort of soft decoding from this information. Should modestly improve the coding
     * gain vs. hard decoding (roughly 0.2-0.3dB).
     * @param enc_msg Byte vector of encoded data.
     * @param dec_msg Byte vector of decoded data.
     * @param snr LoRa whole-packet signal-to-noise ratio (SNR).
     */
    virtual ssize_t decodeSoft(const vector<uint8_t> &enc_msg, vector<uint8_t> &dec_msg, const float snr) {
        return decode(enc_msg, dec_msg);
    }

    void benchmark(size_t num_iters);
};


/**
 * Derived class that just applies/removes the interleaving from the data.
 */
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


/**
 * Derived class that uses convolutional coding.
 */
class FECConv: public FEC {
protected:
    // Convolutional coding parameters
    size_t inv_rate;
    size_t order;
    correct_convolutional *corr_con;    

public:    
    /** 
     * Default constructor. Creates an FECConv object with 1/2 rate and n=9.
     */
    FECConv(void) : FECConv(2, 9) { }

    /**
     * Constructor parameterizable with coding rate and order.
     * @param inv_rate Coding rate. 2 and 3 are currently the only rates implemented.
     * @param order Order of the coder. Values supported are 6, 7, 8, and 9.
     */
    FECConv(const size_t inv_rate, const size_t order);

    /// Destructor.
    ~FECConv(void) {
        correct_convolutional_destroy(corr_con);
    }

    size_t getEncSize(const size_t msg_len) {
        return correct_convolutional_encode_len(corr_con, msg_len)/8;
    }

    size_t encode(const vector<uint8_t> &msg, vector<uint8_t> &enc_msg) {
        enc_msg.resize(FECConv::getEncSize(msg.size()));
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


/**
 * Derived class that implements Reed-Solomon-Viterbi (RSV)
 * forward error correction. The convolutional coding is the same as 
 * implemented in the FECConv class, and the Reed-Solomon outer code
 * is a (256,223) code.
 */
class FECRSV: public FECConv {
protected:
    size_t rs_corr_bytes;
    correct_reed_solomon *rs_con;
    vector<uint8_t> rs_buf;

public:
    /**
     * Constructor. 
     * @param inv_rate Convolutional coding rate.
     * @param order Convolutional coding order.
     * @param my_rs_corr_bytes Number of Reed-Solomon correction bytes.
     */
    FECRSV(const size_t inv_rate, const size_t order, const size_t my_rs_corr_bytes) 
        : FECConv(inv_rate, order) {
        name = "RSV";
        rs_corr_bytes = my_rs_corr_bytes;
        rs_con = correct_reed_solomon_create(correct_rs_primitive_polynomial_ccsds,
                                                  1, 1, rs_corr_bytes);
    }

    /**
     * Default constructor. Initializes with a convolutional coding rate of 2,
     * n=9, and 32 Reed-Solomon correction bytes.
     */
    FECRSV(void) : FECRSV(2, 9, 32) { };

    /// Destructor.
    ~FECRSV(void) {
        FECConv::~FECConv();
        correct_reed_solomon_destroy(rs_con);
    }

    size_t encode(const vector<uint8_t> &msg, vector<uint8_t> &rsv_enc_msg);

    ssize_t decode(const vector<uint8_t> &rsv_enc_msg, vector<uint8_t> &dec_msg);

    size_t getEncSize(const size_t msg_len) {
        size_t enc_size = FECConv::getEncSize(msg_len+rs_corr_bytes);
        MBED_ASSERT(enc_size <= 256);
        return enc_size;
    }
};

#endif /* FEC_HPP */