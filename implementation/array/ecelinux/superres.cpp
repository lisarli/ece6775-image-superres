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

void dut(hls::stream<pixel_type> &strm_in, hls::stream<pixel_type> &strm_out) {
  pixel_type input_image[ORIG_HEIGHT][ORIG_WIDTH][3];
  pixel_type output_image[ORIG_HEIGHT * SCALE_FACTOR][ORIG_WIDTH * SCALE_FACTOR][3];

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

void superres_xcel(pixel_type input_image[ORIG_HEIGHT][ORIG_WIDTH][3], pixel_type output_image[ORIG_HEIGHT * SCALE_FACTOR][ORIG_WIDTH * SCALE_FACTOR][3]) {
  pixel_type EDGE_SHARPENING_KERNEL[5][5] = {
        {-0.00391, -0.01563, -0.02344, -0.0156, -0.00391},
        {-0.01563, -0.06250, -0.09375, -0.06250, -0.01563},
        {-0.02344, -0.09375, 1.85980, -0.09375, -0.02344},
        {-0.01563, -0.06250, -0.09375, -0.06250, -0.01563},
        {-0.00391, -0.01563, -0.02344, -0.0156, -0.00391},
  };
    
  upsample<ORIG_HEIGHT, ORIG_WIDTH, SCALE_FACTOR>(input_image, output_image);

  convolve_and_relu<ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR, 5>(output_image, EDGE_SHARPENING_KERNEL);
  // relu<ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR>(output_image);
  convolve_and_relu<ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR, 5>(output_image, EDGE_SHARPENING_KERNEL);
  // relu<ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR>(output_image);
  convolve_and_relu<ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR, 5>(output_image, EDGE_SHARPENING_KERNEL);
  // relu<ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR>(output_image);
}
