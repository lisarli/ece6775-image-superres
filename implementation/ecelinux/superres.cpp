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
  pixel_type input_image[BLOCK_SIZE][BLOCK_SIZE][3];
  pixel_type output_image[BLOCK_SIZE * SCALE_FACTOR][BLOCK_SIZE * SCALE_FACTOR][3];

  // for(int i=0; i < IMAGE_DIM/BLOCK_SIZE; i++){
  //   FOR_PIXELS(r, c, chan, BLOCK_SIZE, BLOCK_SIZE) {
  //     input_image[r][c][chan] = strm_in.read();
  //   }

  //   superres_xcel(input_image, output_image);

  //   FOR_PIXELS(r, c, chan, BLOCK_SIZE * SCALE_FACTOR, BLOCK_SIZE * SCALE_FACTOR) {
  //     strm_out.write(output_image[r][c][chan]);
  //   }
  // }

  FOR_PIXELS(r, c, chan, BLOCK_SIZE, BLOCK_SIZE) {
    input_image[r][c][chan] = strm_in.read();
  }

  superres_xcel(input_image, output_image);

  FOR_PIXELS(r, c, chan, BLOCK_SIZE * SCALE_FACTOR, BLOCK_SIZE * SCALE_FACTOR) {
    strm_out.write(output_image[r][c][chan]);
  }
}

//----------------------------------------------------------
// superres Accelerator
//----------------------------------------------------------
// @param[in] : input - the testing instance
// @return : the predicted digit

void superres_xcel(pixel_type input_image[BLOCK_SIZE][BLOCK_SIZE][3], pixel_type output_image[BLOCK_SIZE * SCALE_FACTOR][BLOCK_SIZE * SCALE_FACTOR][3]) {
  pixel_type EDGE_SHARPENING_KERNEL[5][5] = {
        {-0.00391, -0.01563, -0.02344, -0.0156, -0.00391},
        {-0.01563, -0.06250, -0.09375, -0.06250, -0.01563},
        {-0.02344, -0.09375, 1.85980, -0.09375, -0.02344},
        {-0.01563, -0.06250, -0.09375, -0.06250, -0.01563},
        {-0.00391, -0.01563, -0.02344, -0.0156, -0.00391},
  };
    
  upsample<BLOCK_SIZE, BLOCK_SIZE, SCALE_FACTOR>(input_image, output_image);

  convolve<BLOCK_SIZE * SCALE_FACTOR, BLOCK_SIZE * SCALE_FACTOR, 5>(output_image, EDGE_SHARPENING_KERNEL);
  relu<BLOCK_SIZE * SCALE_FACTOR, BLOCK_SIZE * SCALE_FACTOR>(output_image);
  convolve<BLOCK_SIZE * SCALE_FACTOR, BLOCK_SIZE * SCALE_FACTOR, 5>(output_image, EDGE_SHARPENING_KERNEL);
  relu<BLOCK_SIZE * SCALE_FACTOR, BLOCK_SIZE * SCALE_FACTOR>(output_image);
  convolve<BLOCK_SIZE * SCALE_FACTOR, BLOCK_SIZE * SCALE_FACTOR, 5>(output_image, EDGE_SHARPENING_KERNEL);
  relu<BLOCK_SIZE * SCALE_FACTOR, BLOCK_SIZE * SCALE_FACTOR>(output_image);
}
