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

void dut(hls::stream<float> &strm_in, hls::stream<float> &strm_out) {
  float input_image[ORIG_HEIGHT][ORIG_WIDTH][3];
  float output_image[ORIG_HEIGHT * SCALE_FACTOR][ORIG_WIDTH * SCALE_FACTOR][3];

  FOR_PIXELS(r, c, chan, ORIG_HEIGHT, ORIG_WIDTH) {
    input_image[r][c][chan] = strm_in.read();
  }

  superres_xcel(input_image, output_image);

  FOR_PIXELS(r, c, chan, ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR) {
    strm_out.write(output_image[r][c][chan]);
  }
}

//----------------------------------------------------------
// superres Accelerator
//----------------------------------------------------------
// @param[in] : input - the testing instance
// @return : the predicted digit

void superres_xcel(float input_image[ORIG_HEIGHT][ORIG_WIDTH][3], float output_image[ORIG_HEIGHT * SCALE_FACTOR][ORIG_WIDTH * SCALE_FACTOR][3]) {
  constexpr float EDGE_SHARPENING_KERNEL[5][5] = {
        {-0.00391, -0.01563, -0.02344, -0.0156, -0.00391},
        {-0.01563, -0.06250, -0.09375, -0.06250, -0.01563},
        {-0.02344, -0.09375, 1.85980, -0.09375, -0.02344},
        {-0.01563, -0.06250, -0.09375, -0.06250, -0.01563},
        {-0.00391, -0.01563, -0.02344, -0.0156, -0.00391},
    };
  
  // we swap between output_image and buffer for convolution
  float buffer[ORIG_HEIGHT * SCALE_FACTOR][ORIG_WIDTH * SCALE_FACTOR][3] = {0.0f};

  upsample<ORIG_HEIGHT, ORIG_WIDTH, SCALE_FACTOR>(input_image, buffer);

  convolve<ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR, 5>(buffer, output_image, EDGE_SHARPENING_KERNEL);
  relu<ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR>(output_image);
  convolve<ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR, 5>(output_image, buffer, EDGE_SHARPENING_KERNEL);
  relu<ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR>(buffer);
  convolve<ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR, 5>(buffer, output_image, EDGE_SHARPENING_KERNEL);
  relu<ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR>(output_image);
}
