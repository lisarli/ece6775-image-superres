//==========================================================================
// superres.cpp
//==========================================================================
// @brief: A convolution kernel for CNN on digit recognition

#include "superres.h"


// write rgb to individual streams
void send_vals(hls::stream<pixel_type> &strm_in, hls::stream<pixel_type> &superres_in0, hls::stream<pixel_type> &superres_in1, hls::stream<pixel_type> &superres_in2){
  for(int r = 0; r < ORIG_DIM; r++){
    for(int c = 0; c < ORIG_DIM; c++){
      superres_in0.write(strm_in.read());
      superres_in1.write(strm_in.read());
      superres_in2.write(strm_in.read());
    }
  }
}

// combine channel results to output stream
void write_vals(hls::stream<pixel_type> &strm_out, hls::stream<pixel_type> &superres_out0, hls::stream<pixel_type> &superres_out1, hls::stream<pixel_type> &superres_out2){
  for(int r = 0; r < OUT_DIM; r++){
    for(int c = 0; c < OUT_DIM; c++){
      strm_out.write(superres_out0.read());
      strm_out.write(superres_out1.read());
      strm_out.write(superres_out2.read());
    }
  }
}

//----------------------------------------------------------
// Top function
//----------------------------------------------------------

void dut(hls::stream<pixel_type> &strm_in, hls::stream<pixel_type> &strm_out){
  #pragma HLS dataflow
  hls::stream<pixel_type> superres_in0;
  hls::stream<pixel_type> superres_in1;
  hls::stream<pixel_type> superres_in2;
  hls::stream<pixel_type> superres_out0;
  hls::stream<pixel_type> superres_out1;
  hls::stream<pixel_type> superres_out2;

  #pragma HLS stream variable=superres_in0 depth=410
  #pragma HLS stream variable=superres_out0 depth=800
  #pragma HLS stream variable=superres_in1 depth=410
  #pragma HLS stream variable=superres_out1 depth=800
  #pragma HLS stream variable=superres_in2 depth=410
  #pragma HLS stream variable=superres_out2 depth=800

  send_vals(strm_in, superres_in0, superres_in1, superres_in2);

  // superres each channel
  superres_xcel(superres_in0, superres_out0);
  superres_xcel(superres_in1, superres_out1);
  superres_xcel(superres_in2, superres_out2);

  write_vals(strm_out, superres_out0, superres_out1, superres_out2);
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
  
  #pragma HLS stream variable=upsample_out depth=64
  #pragma HLS stream variable=layer0_out depth=64
  #pragma HLS stream variable=layer1_out depth=64

  upsample<ORIG_DIM, SCALE_FACTOR>(input_image, upsample_out);

  convolve<ORIG_DIM*SCALE_FACTOR, CONV_DIM0, K_DIM>(upsample_out, layer0_out, EDGE_SHARPENING_KERNEL0);
  convolve<CONV_DIM0, CONV_DIM1, K_DIM>(layer0_out, layer1_out, EDGE_SHARPENING_KERNEL1);
  convolve<CONV_DIM1, OUT_DIM, K_DIM>(layer1_out, output_image, EDGE_SHARPENING_KERNEL2);
}
