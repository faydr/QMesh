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
#include "params.hpp"
#include "correct.h"
#include "fec.hpp"
#include "serial_data.hpp"
#include <cmath>
#include <cstdlib>
#include <cstring>

void testFEC(void) {
    vector<shared_ptr<FEC>> test_fecs;
    shared_ptr<FEC> fec_sptr;
    fec_sptr = make_shared<FEC>();
    test_fecs.push_back(fec_sptr);
    fec_sptr = make_shared<FECInterleave>();
    test_fecs.push_back(fec_sptr);
    fec_sptr = make_shared<FECConv>();
    test_fecs.push_back(fec_sptr);
    fec_sptr = make_shared<FECRSV>();
    test_fecs.push_back(fec_sptr);
    
    debug_printf(DBG_INFO, "Initialized FEC objects\r\n");
    ThisThread::sleep_for(1000);

    for(vector<shared_ptr<FEC>>::iterator iter = test_fecs.begin(); iter != test_fecs.end(); iter++) {
        debug_printf(DBG_INFO, "====================\r\n");
        debug_printf(DBG_INFO, "Now testing %s for correctness...\r\n", iter->get()->getName().c_str());
        ThisThread::sleep_for(1000);
        // Make a random string
        int fec_success = 0;
        int fec_fail = 0;
        int fec_total = 0;
        for(int i = 0; i < 100; i++) {
            int pld_len = radio_cb["Payload Length"].get<int>();
            vector<uint8_t> rand_data(pld_len);
            std::generate_n(rand_data.begin(), pld_len, rand);
            auto test_frame = make_shared<Frame>(*iter);
            test_frame->loadTestFrame(rand_data);
            auto serialized_data = make_shared<vector<uint8_t>>();
            test_frame->serialize(*serialized_data);         
            auto test_output_frame = make_shared<Frame>(*iter);           
            test_output_frame->deserialize(serialized_data);     
            if(*test_frame != *test_output_frame) {
                fec_fail += 1;
            }
            else {
                fec_success += 1;
            }
            fec_total += 1;
        }
        debug_printf(DBG_INFO, "Finished testing. FEC succeeded %d times, failed %d times\r\n", 
            fec_success, fec_fail);  
        debug_printf(DBG_INFO, "====================\r\n");
        debug_printf(DBG_INFO, "Now testing %s for bit error resilience...\r\n", iter->get()->getName().c_str());
        ThisThread::sleep_for(1000);
        Frame size_frame(*iter);
        debug_printf(DBG_INFO, "Size of frame is %d\r\n", size_frame.getFullPktSize());
        fec_success = 0;
        fec_fail = 0;
        fec_total = 0;
        for(int i = 0; i < size_frame.getFullPktSize(); i++) {
            for(int j = 0; j < 10; j++) {
                int pld_len = radio_cb["Payload Length"].get<int>();
                vector<uint8_t> rand_data(pld_len);
                std::generate_n(rand_data.begin(), pld_len, rand);
                auto test_frame = make_shared<Frame>(*iter);
                test_frame->loadTestFrame(rand_data);
                vector<uint8_t> serialized_data;
                test_frame->serialize(serialized_data);
                serialized_data[i] = rand();
                auto test_output_frame = make_shared<Frame>(*iter);
                auto deserialized_data = make_shared<vector<uint8_t>>(serialized_data);
                test_output_frame->deserialize(deserialized_data);
                if(*test_frame != *test_output_frame) {
                    fec_fail += 1;
                }
                else {
                    fec_success += 1;
                }
                fec_total += 1;
            }       
        }   
        debug_printf(DBG_INFO, "Finished testing. FEC succeeded %d times, failed %d times, tested %d times\r\n", 
            fec_success, fec_fail, fec_total);     
        debug_printf(DBG_INFO, "====================\r\n");
        debug_printf(DBG_INFO, "Now benchmarking performance...\r\n");
        (*iter)->benchmark(100);
    }
}

size_t FECConv::getEncSize(const size_t msg_len) {
    int conv_len = correct_convolutional_encode_len(corr_con, msg_len);
    //debug_printf(DBG_INFO, "conv len is %d\r\n", conv_len);
    return (size_t) ceilf((float) correct_convolutional_encode_len(corr_con, msg_len)/8.0f);
}

FECConv::FECConv(const size_t inv_rate, const size_t order) {
    name = "Convolutional Coding";
    // Set up the convolutional outer code
    this->inv_rate = inv_rate;
    this->order = order;
    correct_convolutional_polynomial_t *poly = NULL;
    switch(inv_rate) {
        case 2: // 1/2
            switch(order) {
                case 6: poly = conv_r12_6_polynomial; break;
                case 7: poly = libfec_r12_7_polynomial; break;
                case 8: poly = conv_r12_8_polynomial; break;
                case 9: poly = conv_r12_9_polynomial; break;
                default:
                    debug_printf(DBG_ERR, "Invalid convolutional coding parameters selected\r\n");
                    MBED_ASSERT(false);
                break;
            }
        break;
        case 3: // 1/3
            switch(order) {
                case 6: poly = conv_r13_6_polynomial; break;
                case 7: poly = conv_r13_7_polynomial; break;
                case 8: poly = conv_r13_8_polynomial; break;
                case 9: poly = libfec_r13_9_polynomial; break;
                default:
                    debug_printf(DBG_ERR, "Invalid convolutional coding parameters selected\r\n");
                    MBED_ASSERT(false);
                break;
            }
        break;
        case 6: // 1/6
            if(order == 15) {
                poly = libfec_r16_15_polynomial;
            }
            else {
                debug_printf(DBG_ERR, "Invalid convolutional coding parameters selected\r\n");
                MBED_ASSERT(false);                    
            }
        break;
        default:
            debug_printf(DBG_ERR, "Invalid convolutional coding parameters selected\r\n");
            MBED_ASSERT(false);
        break;
    }
    corr_con = correct_convolutional_create(inv_rate, order, poly);
}


size_t FECConv::encode(const vector<uint8_t> &msg, vector<uint8_t> &enc_msg) {
    enc_msg.resize(FECConv::getEncSize(msg.size()));
    vector<uint8_t> msg_int(msg.size());
    //interleaveBits(msg, msg_int);
    msg_int = msg;
    size_t enc_len = (size_t) ceilf((float) correct_convolutional_encode(corr_con, msg_int.data(), 
            msg_int.size(), enc_msg.data())/8.0f);
    //debug_printf(DBG_INFO, "Encode length is %d bytes\r\n", enc_len);
    size_t enc_msg_size_bits = correct_convolutional_encode_len(corr_con, Frame::size());
    //debug_printf(DBG_INFO, "Encode length is %d bits\r\n", enc_msg_size_bits);
    return enc_len;
}


ssize_t FECConv::decode(const vector<uint8_t> &enc_msg, vector<uint8_t> &dec_msg) {
    dec_msg.resize(Frame::size());
    vector<uint8_t> dec_msg_int(dec_msg.size(), 0);
    size_t enc_msg_size_bits = correct_convolutional_encode_len(corr_con, Frame::size());
    size_t dec_size = correct_convolutional_decode(corr_con, enc_msg.data(), 
                            enc_msg_size_bits, dec_msg_int.data());
    //debug_printf(DBG_INFO, "Decode length is %d bytes\r\n", dec_size);
    //debug_printf(DBG_INFO, "Decode length is %d bits\r\n", enc_msg_size_bits);
    //deinterleaveBits(dec_msg_int, dec_msg);
    dec_msg = dec_msg_int;
    return dec_size;
}

void FEC::benchmark(size_t num_iters) {
    debug_printf(DBG_INFO, "====================\r\n");
    debug_printf(DBG_INFO, "Now benchmarking the %s. Running for %d iterations\r\n", name.c_str(), num_iters);
    debug_printf(DBG_INFO, "Current frame size is %d\r\n", Frame::size());
    vector<uint8_t> msg_data(Frame::size());
    vector<uint8_t> enc_data(Frame::size());
    debug_printf(DBG_INFO, "Benchmarking the encode...\r\n");
    Timer enc_timer;
    enc_timer.start();
    for(size_t i = 0; i < num_iters; i++) {
        encode(msg_data, enc_data);
    }
    enc_timer.stop();
    int enc_num_ms = enc_timer.read_ms();
    debug_printf(DBG_INFO, "Done!\r\n");
    debug_printf(DBG_INFO, "Benchmarking the decode...\r\n");
    Timer dec_timer;;
    size_t enc_size = getEncSize(Frame::size());
    dec_timer.start();
    for(size_t i = 0; i < num_iters; i++) {
        decode(enc_data, msg_data);
    }
    dec_timer.stop();
    int dec_num_ms = dec_timer.read_ms();
    debug_printf(DBG_INFO, "Done!\r\n");        
    debug_printf(DBG_INFO, "Benchmarking complete! \r\n");
    debug_printf(DBG_INFO, "Encode: %d iterations complete in %d ms\r\n", num_iters, enc_num_ms);
    float enc_ms_per_iter = (float) enc_num_ms / (float) num_iters;
    float enc_iters_per_sec = (float) num_iters / ((float) enc_num_ms / 1000.f);
    debug_printf(DBG_INFO, "Encode: %f ms/iteration, %f iterations/s\r\n", enc_ms_per_iter, enc_iters_per_sec);
    debug_printf(DBG_INFO, "Decode: %d iterations complete in %d ms\r\n", num_iters, dec_num_ms);
    float dec_ms_per_iter = (float) dec_num_ms / (float) num_iters;
    float dec_iters_per_sec = (float) num_iters / ((float) dec_num_ms / 1000.f);
    debug_printf(DBG_INFO, "Decode: %f ms/iteration, %f iterations/s\r\n", dec_ms_per_iter, dec_iters_per_sec);        
    debug_printf(DBG_INFO, "====================\r\n");
}


size_t FECRSV::encode(const vector<uint8_t> &msg, vector<uint8_t> &rsv_enc_msg) {
    MBED_ASSERT(getEncSize(msg.size()) <= 256);
    vector<uint8_t> rs_enc_msg(msg.size()+rs_corr_bytes);
    size_t rs_size = correct_reed_solomon_encode(rs_con, msg.data(), msg.size(), rs_enc_msg.data());
    rsv_enc_msg.resize(FECConv::getEncSize(rs_enc_msg.size()));
    size_t conv_len = FECConv::encode(rs_enc_msg, rsv_enc_msg);
    //debug_printf(DBG_INFO, "conv_len %d, rsv_enc_msg.size %d\r\n", conv_len, rsv_enc_msg.size());
    MBED_ASSERT(rsv_enc_msg.size() == conv_len);
    return conv_len;
}


ssize_t FECRSV::decode(const vector<uint8_t> &rsv_enc_msg, vector<uint8_t> &dec_msg) {
    //debug_printf(DBG_INFO, "Now RSV decoding...\r\n");
    vector<uint8_t> rs_enc_msg(rsv_enc_msg.size());
    //debug_printf(DBG_INFO, "Now rsv_enc_msg.size...%d\r\n", rsv_enc_msg.size());
    size_t conv_bytes = correct_convolutional_decode(corr_con, rsv_enc_msg.data(), 
                            rsv_enc_msg.size()*8, rs_enc_msg.data());
    //debug_printf(DBG_INFO, "Now conv decoded...%d %d\r\n", conv_bytes, rs_enc_msg.size());
    //MBED_ASSERT(conv_bytes != -1);
    //rs_enc_msg.resize(conv_bytes);
    dec_msg.resize(conv_bytes-rs_corr_bytes);
    //debug_printf(DBG_INFO, "Now rs decoding\r\n");    
    size_t rs_len = correct_reed_solomon_decode(rs_con, rs_enc_msg.data(), conv_bytes, 
                        dec_msg.data());
    //debug_printf(DBG_INFO, "conv_len %d, dec_msg.size %d\r\n", rs_len, dec_msg.size());
    //ThisThread::sleep_for(250);                       
    //MBED_ASSERT(dec_msg.size() == rs_len);
    return dec_msg.size();
}


// Equation 4 in https://arxiv.org/pdf/1705.05899.pdf shows how to 
// calculate the BER from the SNR and LoRa parameters (SF and CR)
// The basic equation is: log 10(BER(SNRdB)) =α*exp (β*SNRdB)
/* 
SF | CR | α           | β      | rsquare | SNR cut-off (dB)
------------------------------------------------------------
7  | 1  | -30.2580    | 0.2857 | 0.9997  | -12.283373
7  | 3  | -105.1966   | 0.3746 | 0.9999  | -12.696281
8  | 1  | -77.1002    | 0.2993 | 0.9999  | -14.848583
8  | 3  | -289.8133   | 0.3756 | 0.9995  | -15.358891
9  | 1  | -244.6424   | 0.3223 | 0.9993  | -17.374993
9  | 3  | -1114.3312  | 0.3969 | 0.9994  | -17.9260101
10 | 1  | -725.9556   | 0.3340 | 0.9996  | -20.0254103
10 | 3  | -4285.4440  | 0.4116 | 0.9991  | -20.5581111
11 | 1  | -2109.8064  | 0.3407 | 1.0000  | -22.7568113
11 | 3  | -20771.6945 | 0.4332 | 0.9996  | -23.1791121
12 | 1  | -4452.3653  | 0.3317 | 0.9986  | -25.6243123
12 | 3  | -98658.1166 | 0.4485 | 0.9993  | -25.8602
*/
static map<pair<int, int>, tuple<float, float, float, float> > ber_plot = {
    {{7, 1}, {-30.2580, 0.2857, 0.997, -12.283373}},
    {{7, 3}, {-105.1966, 0.3746, 0.9999, -12.696281}},
    {{8, 1}, {-77.1002, 0.2993, 0.9999, -14.848583}},
    {{8, 3}, {-289.8133, 0.3856, 0.9995, -15.358891}},
    {{9, 1}, {-244.6424, 0.3223, 0.9993, -17.374993}},   
    {{9, 3}, {-1114.3312, 0.3969, 0.9994, -17.9260101}},   
    {{10, 1}, {-725.9556, 0.3340, 0.9996, -20.0254103}},         
    {{10, 3}, {-4285.4440, 0.4116, 0.9991, -20.5581111}},
    {{11, 1}, {-2109.8064, 0.3407, 1.0000, -22.7568113}},
    {{11, 3}, {-20771.6945, 0.4332, 0.9996, -23.1791121}},
    {{12, 1}, {-4452.3653, 0.3317, 0.9986, -25.6243123}},
    {{12, 3}, {-98658.1166, 0.4485, 0.9993, -25.8602}},
};
float FEC::getBER(const float snr) {
    int sf = radio_cb["SF"].get<int>();
    int cr = radio_cb["CR"].get<int>();
    auto constants = ber_plot[make_tuple<float, float>(sf, cr)];
    float rhs = get<0>(constants) * expf(get<1>(constants)*snr);
    return powf(10.f, rhs);
}
