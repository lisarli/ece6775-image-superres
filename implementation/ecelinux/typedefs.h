//===========================================================================
// typedefs.h
//===========================================================================
// @brief: This header defines the shorthand of several ap_uint data types.

#ifndef TYPEDEFS
#define TYPEDEFS

#include <ap_int.h>

typedef bool bit;
typedef ap_int<8> bit8_t;
typedef ap_int<16> bit16_t;
typedef ap_uint<2> bit2_t;
typedef ap_uint<4> bit4_t;
typedef ap_uint<32> bit32_t;

typedef ap_fixed<32,8> pixel_type;

const int SCALE_FACTOR = 2;
const int ORIG_WIDTH = 20;
const int ORIG_HEIGHT = 20;

#define FOR_PIXELS(r, c, chan, H, W) \
    for (int r = 0; r < H; ++r) \
        for (int c = 0; c < W; ++c) \
            for (int chan = 0; chan < 3; ++chan)

#endif
