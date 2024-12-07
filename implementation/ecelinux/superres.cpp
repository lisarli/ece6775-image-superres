//==========================================================================
// superres.cpp
//==========================================================================
// @brief: A convolution kernel for CNN on digit recognition

#include "superres.h"
#include "layer.h"
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std;

//----------------------------------------------------------
// Top function
//----------------------------------------------------------

void dut(hls::stream<bit32_t> &strm_in, hls::stream<bit32_t> &strm_out) {
  pixel_type input_image[ORIG_HEIGHT][ORIG_WIDTH][3];
  pixel_type output_image[ORIG_HEIGHT * SCALE_FACTOR][ORIG_WIDTH * SCALE_FACTOR][3];
  pixel_type pixel_in;
  bit32_t bits_out;

  FOR_PIXELS(r, c, chan, ORIG_HEIGHT, ORIG_WIDTH) {
    pixel_in(31,0) = strm_in.read();
    input_image[r][c][chan] = pixel_in;
  }

  superres_xcel(input_image, output_image);

  FOR_PIXELS(r, c, chan, ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR) {
    bit32_t bits_out = output_image[r][c][chan](31,0);
    strm_out.write(bits_out);
  }
}

//----------------------------------------------------------
// superres Accelerator
//----------------------------------------------------------
// @param[in] : input - the testing instance
// @return : the predicted digit

void superres_xcel(pixel_type input_image[ORIG_HEIGHT][ORIG_WIDTH][3], pixel_type output_image[ORIG_HEIGHT * SCALE_FACTOR][ORIG_WIDTH * SCALE_FACTOR][3]) {
  pixel_type EDGE_SHARPENING_KERNEL[5][5] = {
        {-0.00391, -0.01563, -0.02344, -0.0156, -0.00391},
        {-0.01563, -0.06250, -0.09375, -0.06250, -0.01563},
        {-0.02344, -0.09375, 1.85980, -0.09375, -0.02344},
        {-0.01563, -0.06250, -0.09375, -0.06250, -0.01563},
        {-0.00391, -0.01563, -0.02344, -0.0156, -0.00391},
  };
    
  upsample<ORIG_HEIGHT, ORIG_WIDTH, SCALE_FACTOR>(input_image, output_image);

  convolve<ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR, 5>(output_image, EDGE_SHARPENING_KERNEL);
  convolve<ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR, 5>(output_image, EDGE_SHARPENING_KERNEL);
  convolve<ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR, 5>(output_image, EDGE_SHARPENING_KERNEL);
}
