//===========================================================================
// layer.h
//===========================================================================
// @brief: This header file defines the interface for the core functions.

#ifndef LAYER_H
#define LAYER_H

#include "typedefs.h"

template <int H, int W, int S>
void upsample(pixel_type input[H][W][3], pixel_type output[H * S][W * S][3])
{
  #pragma HLS array_partition variable=input complete dim=3
  #pragma HLS array_partition variable=output complete dim=3
  #pragma HLS array_partition variable=input cyclic factor=10 dim=2
  #pragma HLS array_partition variable=output cyclic factor=10 dim=2
  for (int r = 0; r < H*S; ++r){
    #pragma HLS pipeline
    for (int c = 0; c < W*S; ++c){
      #pragma HLS unroll
      for (int chan = 0; chan < 3; ++chan){
        #pragma HLS unroll
        output[r][c][chan] = input[r / S][c / S][chan];
      }
    }
  }
}

template <int H, int W, int C>
void initialize_memory(pixel_type input[H][W][3]) {
  #pragma HLS array_partition variable=input complete dim=3
  #pragma HLS array_partition variable=input cyclic factor=10 dim=2
  for (int r = 0; r < ORIG_HEIGHT * SCALE_FACTOR; ++r){
    #pragma HLS pipeline
    for (int c = 0; c < ORIG_WIDTH * SCALE_FACTOR; ++c){
      #pragma HLS unroll
      for (int chan = 0; chan < 3; ++chan){
      #pragma HLS unroll
        input[r][c][chan] = C;
      }
    }
  }
}

template <int H, int W, int KS>
void convolve_and_relu(pixel_type buffer[H][W][3], const pixel_type kernel[KS][KS])
{
  #pragma HLS array_partition variable=buffer complete dim=3
  #pragma HLS array_partition variable=buffer cyclic factor=10 dim=2
  #pragma HLS array_partition variable=kernel complete dim=0

  pixel_type output[H][W][3];
  initialize_memory<H,W,0>(output);
  
  for (int i = KS / 2; i < H - KS / 2; ++i) {
    for (int j = KS / 2; j < W - KS / 2; ++j) {
      #pragma HLS pipeline
      for (int ki = -KS / 2; ki <= KS / 2; ++ki) {
        #pragma HLS unroll
        for (int kj = -KS / 2; kj <= KS / 2; ++kj) {
          #pragma HLS unroll
          output[i][j][0] += buffer[i + ki][j + kj][0] * kernel[ki + KS / 2][kj + KS / 2];
          output[i][j][1] += buffer[i + ki][j + kj][1] * kernel[ki + KS / 2][kj + KS / 2];
          output[i][j][2] += buffer[i + ki][j + kj][2] * kernel[ki + KS / 2][kj + KS / 2];
        }
      }
    }
  }

  for (int r = 0; r < H; ++r){
    #pragma HLS pipeline
    for (int c = 0; c < W; ++c){
      #pragma HLS unroll
      for (int chan = 0; chan < 3; ++chan){
        #pragma HLS unroll
        // does the relu on the output 
        buffer[r][c][chan] = min(1.0, max(0.0, output[r][c][chan]));
      }
    }
  }
}

inline pixel_type min(pixel_type a, pixel_type b) {
  return a < b ? a : b;
}

inline pixel_type max(pixel_type a, pixel_type b) {
  return a > b ? a : b;
}

// template <int H, int W>
// void relu(pixel_type buffer[H][W][3]) {
//   #pragma HLS array_partition variable=buffer complete dim=3
//   #pragma HLS array_partition variable=buffer cyclic factor=10 dim=2

//   for (int r = 0; r < H; ++r){
//     #pragma HLS pipeline
//     for (int c = 0; c < W; ++c){
//       #pragma HLS unroll
//       for (int chan = 0; chan < 3; ++chan){
//         #pragma HLS unroll
//         buffer[r][c][chan] = min(1.0, max(0.0, buffer[r][c][chan]));
//       }
//     }
//   }
// }

#endif
