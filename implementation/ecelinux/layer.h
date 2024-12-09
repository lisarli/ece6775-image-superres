//===========================================================================
// layer.h
//===========================================================================
// @brief: This header file defines the interface for the core functions.

#ifndef LAYER_H
#define LAYER_H

#include "typedefs.h"
#include <hls_stream.h>

template <int D, int S>
void upsample(hls::stream<pixel_type> &strm_in, hls::stream<pixel_type> &strm_out)
{
  pixel_type row[D];
  for(int r = 0; r < D; r++){
    for(int c = 0; c < D; c++){
      row[c] = strm_in.read();
    }

    // replicate
    for(int i = 0; i < S; i++){
      for(int c = 0; c < D; c++){
        for(int j = 0; j < S; j++){
          strm_out.write(row[c]);
        }
      }
    }
  }
}

template <int I, int O, int KS>
void convolve(hls::stream<pixel_type> &strm_in, hls::stream<pixel_type> &strm_out, const pixel_type kernel[KS][KS])
{
  pixel_type linebuf[KS][I];
  #pragma HLS array_partition variable=linebuf dim=1 complete
  pixel_type window[KS][KS];
  #pragma HLS array_partition variable=window dim=0 complete

  // fill linebuf with first KS rows from input
  INIT_LINEBUF:
  for (int i = 0; i < KS; i++) {
    for (int j = 0; j < I; j++) {
      linebuf[i][j] = strm_in.read();
    }
  }

  // fill window with KS pixels from linebuf
  INIT_WINDOW:
  for (int k1 = 0; k1 < KS; k1++) {
    for (int k2 = 0; k2 < KS; k2++) {
      window[k1][k2] = linebuf[k1][k2];
    }
  }

  CONV_ROW:
  for (int i = 0; i < O; i++) {
    CONV_COL:
    for (int j = 0; j < O; j++) {
      pixel_type acc = 0;
      DOT_PRODUCT:
      for (int k1 = 0; k1 < KS; k1++) {
          for (int k2 = 0; k2 < KS; k2++) {
              acc += window[k1][k2] * kernel[k1][k2];
          }
      }
      strm_out.write(acc);

      // update window with the next column of pixels
      if (j < O-1) {
        // shift window pixels left
        for(int k1 = 0; k1 < KS; k1++){
          for(int k2 = 0; k2 < KS-1; k2++){
            window[k1][k2] = window[k1][k2+1];
          }
        }
        // get last col
        for (int k1 = 0; k1 < KS; k1++) {
          window[k1][KS-1] = linebuf[k1][j+KS];
        }
      }
    }

    // update linebuf with the next row from input
    if (i < O-1) {
      UPDATE_LINEBUF:
      // shift linebuf pixels up
      for(int k1 = 0; k1 < KS-1; k1++){
        for(int k2 = 0; k2 < I; k2++){
          linebuf[k1][k2] = linebuf[k1+1][k2];
        }
      }
      for (int j = 0; j < I; j++) {
        linebuf[KS-1][j] = strm_in.read();
      }
    }
    
    // reload window
    RELOAD_WINDOW:
    for (int k1 = 0; k1 < KS; k1++) {
      for (int k2 = 0; k2 < KS; k2++) {
        window[k1][k2] = linebuf[k1][k2];
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

#endif
