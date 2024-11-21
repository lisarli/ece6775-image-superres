//===========================================================================
// superres.h
//===========================================================================
// @brief: This header file defines the interface for the core functions.

#ifndef SUPERRES_H
#define SUPERRES_H
#include "model.h"
#include "typedefs.h"
#include <hls_stream.h>

// Top function for synthesis
void dut(hls::stream<bit32_t> &strm_in, hls::stream<bit32_t> &strm_out);

// Top function for superres accelerator
bit32_t superres_xcel(bit input[1][I_WIDTH1][I_WIDTH1]);

#endif
