//===========================================================================
// superres.h
//===========================================================================
// @brief: This header file defines the interface for the core functions.

#ifndef SUPERRES_H
#define SUPERRES_H
#include "typedefs.h"
#include <hls_stream.h>

// Top function for synthesis
void dut(hls::stream<pixel_type> &strm_in, hls::stream<pixel_type> &strm_out);

// Top function for superres accelerator
void superres_xcel(hls::stream<pixel_type> &input_image, hls::stream<pixel_type> &output_image);

#endif
