//==========================================================================
// superres.cpp
//==========================================================================
// @brief: A convolution kernel for CNN on digit recognition

#include "superres.h"

//----------------------------------------------------------
// Top function
//----------------------------------------------------------

void dut(hls::stream<pixel_type> &strm_in, hls::stream<pixel_type> &strm_out){
  #pragma HLS dataflow
  hls::stream<pixel_type> superres_in[3];
  hls::stream<pixel_type> superres_out[3];

  // write rgb to individual streams
  for(int r = 0; r < ORIG_DIM; r++){
    for(int c = 0; c < ORIG_DIM; c++){
      for(int i = 0; i < 3; i++){
        superres_in[i].write(strm_in.read());
      }
    }
  }

  // superres each channel
  for(int i = 0; i < 3; i++){
    superres_xcel(superres_in[i],superres_out[i]);
  }

  // combine channel results to output stream
  for(int r = 0; r < OUT_DIM; r++){
    for(int c = 0; c < OUT_DIM; c++){
      for(int i = 0; i < 3; i++){
        strm_out.write(superres_out[i].read());
      }
    }
  }
}

//----------------------------------------------------------
// superres Accelerator
//----------------------------------------------------------
// @param[in] : input - the testing instance
// @return : the predicted digit

void superres_xcel(hls::stream<pixel_type> &input_image, hls::stream<pixel_type> &output_image){
  #pragma HLS dataflow
  pixel_type EDGE_SHARPENING_KERNEL0[5][5] = {
        {-0.00391, -0.01563, -0.02344, -0.0156, -0.00391},
        {-0.01563, -0.06250, -0.09375, -0.06250, -0.01563},
        {-0.02344, -0.09375, 1.85980, -0.09375, -0.02344},
        {-0.01563, -0.06250, -0.09375, -0.06250, -0.01563},
        {-0.00391, -0.01563, -0.02344, -0.0156, -0.00391},
  };
  pixel_type EDGE_SHARPENING_KERNEL1[5][5] = {
        {-0.00391, -0.01563, -0.02344, -0.0156, -0.00391},
        {-0.01563, -0.06250, -0.09375, -0.06250, -0.01563},
        {-0.02344, -0.09375, 1.85980, -0.09375, -0.02344},
        {-0.01563, -0.06250, -0.09375, -0.06250, -0.01563},
        {-0.00391, -0.01563, -0.02344, -0.0156, -0.00391},
  };
  pixel_type EDGE_SHARPENING_KERNEL2[5][5] = {
        {-0.00391, -0.01563, -0.02344, -0.0156, -0.00391},
        {-0.01563, -0.06250, -0.09375, -0.06250, -0.01563},
        {-0.02344, -0.09375, 1.85980, -0.09375, -0.02344},
        {-0.01563, -0.06250, -0.09375, -0.06250, -0.01563},
        {-0.00391, -0.01563, -0.02344, -0.0156, -0.00391},
  };
  
  hls::stream<pixel_type> upsample_out;
  hls::stream<pixel_type> layer0_out;
  hls::stream<pixel_type> layer1_out;
  
  upsample<ORIG_DIM, SCALE_FACTOR>(input_image, upsample_out);

  convolve<ORIG_DIM*SCALE_FACTOR, CONV_DIM0, K_DIM>(upsample_out, layer0_out, EDGE_SHARPENING_KERNEL0);
  convolve<CONV_DIM0, CONV_DIM1, 5>(layer0_out, layer1_out, EDGE_SHARPENING_KERNEL1);
  convolve<CONV_DIM1, OUT_DIM, 5>(layer1_out, output_image, EDGE_SHARPENING_KERNEL2);
}
