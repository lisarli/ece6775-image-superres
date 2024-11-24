//===========================================================================
// superres.h
//===========================================================================
// @brief: This header file defines the interface for the core functions.

#ifndef SUPERRES_H
#define SUPERRES_H
#include "typedefs.h"
#include <hls_stream.h>

// Top function for synthesis
void dut(hls::stream<float> &strm_in, hls::stream<float> &strm_out);

// Top function for superres accelerator
void superres_xcel(float input_image[ORIG_HEIGHT][ORIG_WIDTH][3], float output_image[ORIG_HEIGHT * SCALE_FACTOR][ORIG_WIDTH * SCALE_FACTOR][3]);

#endif
