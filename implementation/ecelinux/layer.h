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
  for (int r = 0; r < H*S; ++r){
    for (int c = 0; c < W*S; ++c){
      for (int chan = 0; chan < 3; ++chan){
        output[r][c][chan] = input[r / S][c / S][chan];
      }
    }
  }
}

template <int H, int W, int C>
void initialize_memory(pixel_type input[H][W][3]) {
  for (int r = 0; r < ORIG_HEIGHT * SCALE_FACTOR; ++r){
    for (int c = 0; c < ORIG_WIDTH * SCALE_FACTOR; ++c){
      for (int chan = 0; chan < 3; ++chan){
        input[r][c][chan] = C;
      }
    }
  }
}

template <int H, int W, int KS>
void convolve(pixel_type buffer[H][W][3], const pixel_type kernel[KS][KS])
{
  pixel_type output[H][W][3];
  initialize_memory<H,W,0>(output);
  
  for (int i = KS / 2; i < H - KS / 2; ++i) {
    for (int j = KS / 2; j < W - KS / 2; ++j) {
      for (int ki = -KS / 2; ki <= KS / 2; ++ki) {
        for (int kj = -KS / 2; kj <= KS / 2; ++kj) {
          output[i][j][0] += buffer[i + ki][j + kj][0] * kernel[ki + KS / 2][kj + KS / 2];
          output[i][j][1] += buffer[i + ki][j + kj][1] * kernel[ki + KS / 2][kj + KS / 2];
          output[i][j][2] += buffer[i + ki][j + kj][2] * kernel[ki + KS / 2][kj + KS / 2];
        }
      }
    }
  }
  FOR_PIXELS(r, c, chan, H, W) {
    buffer[r][c][chan] = output[r][c][chan];
  }
}

inline pixel_type min(pixel_type a, pixel_type b) {
  return a < b ? a : b;
}

inline pixel_type max(pixel_type a, pixel_type b) {
  return a > b ? a : b;
}

template <int H, int W>
void relu(pixel_type buffer[H][W][3]) {
  for (int r = 0; r < H; ++r){
    for (int c = 0; c < W; ++c){
      for (int chan = 0; chan < 3; ++chan){
        buffer[r][c][chan] = min(1.0, max(0.0, buffer[r][c][chan]));
      }
    }
  }
}

#endif
