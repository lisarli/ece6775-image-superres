//===========================================================================
// layer.h 
//===========================================================================
//===========================================================================

#ifndef LAYER_H
#define LAYER_H

#include "typedefs.h"

// Inline Min and Max Functions
inline pixel_type min_val(pixel_type a, pixel_type b) {
    return a < b ? a : b;
}

inline pixel_type max_val(pixel_type a, pixel_type b) {
    return a > b ? a : b;
}

// Upsample Function
template <int H, int W, int S>
void upsample(pixel_type input[H][W][3], pixel_type output[H * S][W * S][3])
{
    #pragma HLS array_partition variable=input complete dim=3
    #pragma HLS array_partition variable=output complete dim=3

    upsample_loop_r: for (int r = 0; r < H * S; ++r) {
        #pragma HLS pipeline II=1
        upsample_loop_c: for (int c = 0; c < W * S; ++c) {
            #pragma HLS unroll
            for (int chan = 0; chan < 3; ++chan) {
                output[r][c][chan] = input[r / S][c / S][chan];
            }
        }
    }
}

// Initialize Memory Function
template <int H, int W, int C>
void initialize_memory(pixel_type input[H][W][3]) {
    #pragma HLS array_partition variable=input complete dim=3

    initialize_loop_r: for (int r = 0; r < H; ++r) {
        #pragma HLS pipeline II=1
        initialize_loop_c: for (int c = 0; c < W; ++c) {
            #pragma HLS unroll
            for (int chan = 0; chan < 3; ++chan) {
                input[r][c][chan] = C;
            }
        }
    }
}

// ReLU Function
template <int H, int W>
void relu(pixel_type buffer[H][W][3]) {
    #pragma HLS array_partition variable=buffer complete dim=3

    relu_loop_r: for (int r = 0; r < H; ++r) {
        #pragma HLS pipeline II=1
        relu_loop_c: for (int c = 0; c < W; ++c) {
            #pragma HLS unroll
            for (int chan = 0; chan < 3; ++chan) {
                buffer[r][c][chan] = min_val((pixel_type)1.0, max_val((pixel_type)0.0, buffer[r][c][chan]));
            }
        }
    }
}

// Convolve Function 
template <int H, int W, int KS>
void convolve(pixel_type buffer[H][W][3], const pixel_type kernel[KS][KS])
{

    #pragma HLS array_partition variable=buffer complete dim=3
    #pragma HLS array_partition variable=kernel complete dim=0

    // Intermediate buffer to store convolution results
    pixel_type intermediate_buffer[H][W][3];
    #pragma HLS array_partition variable=intermediate_buffer complete dim=3

    #pragma HLS inline off

    #pragma HLS allocation instances=mul limit=12 operation

    convolve_loop_i: for (int i = KS / 2; i < H - KS / 2; i += 4) {  // Unroll by 4 for spatial parallelism
        #pragma HLS pipeline II=1
        convolve_loop_j: for (int j = KS / 2; j < W - KS / 2; j += 4) {  // Unroll by 4 for spatial parallelism
            #pragma HLS pipeline II=1
            #pragma HLS unroll
            for (int chan = 0; chan < 3; ++chan) {
                pixel_type acc1 = 0, acc2 = 0, acc3 = 0, acc4 = 0;

                convolve_loop_ki: for (int ki = 0; ki < KS; ++ki) {
                    #pragma HLS pipeline II=1 rewind
                    convolve_loop_kj: for (int kj = 0; kj < KS; ++kj) {
                        acc1 += buffer[i + ki - KS / 2][j + kj - KS / 2][chan] * kernel[ki][kj];
                        acc2 += buffer[i + ki - KS / 2][j + kj - KS / 2 + 1][chan] * kernel[ki][kj];
                        acc3 += buffer[i + ki - KS / 2][j + kj - KS / 2 + 2][chan] * kernel[ki][kj];
                        acc4 += buffer[i + ki - KS / 2][j + kj - KS / 2 + 3][chan] * kernel[ki][kj];
                    }
                }

                // Store the accumulated results in the intermediate buffer
                intermediate_buffer[i][j][chan] = acc1;
                intermediate_buffer[i][j + 1][chan] = acc2;
                intermediate_buffer[i][j + 2][chan] = acc3;
                intermediate_buffer[i][j + 3][chan] = acc4;
            }
        }
    }

    // Write back to original buffer from intermediate_buffer
    write_back_loop_r: for (int r = 0; r < H; ++r) {
        #pragma HLS pipeline II=1
        write_back_loop_c: for (int c = 0; c < W; ++c) {
            #pragma HLS pipeline II=1
            #pragma HLS unroll
            for (int chan = 0; chan < 3; ++chan) {
                buffer[r][c][chan] = intermediate_buffer[r][c][chan];
            }
        }
    }
}

#endif
