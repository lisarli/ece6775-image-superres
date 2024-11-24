//===========================================================================
// superres.h
//===========================================================================
// @brief: This header file defines the interface for the core functions.

#ifndef SUPERRES_H
#define SUPERRES_H
#include "typedefs.h"
#include <hls_stream.h>

// Top function for synthesis
void dut(hls::stream<bit32_t> &strm_in, hls::stream<bit32_t> &strm_out);

// Top function for superres accelerator
void superres_xcel(pixel_type input_image[ORIG_HEIGHT][ORIG_WIDTH][3], pixel_type output_image[ORIG_HEIGHT * SCALE_FACTOR][ORIG_WIDTH * SCALE_FACTOR][3]);

#endif
