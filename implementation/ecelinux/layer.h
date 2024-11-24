//===========================================================================
// layer.h
//===========================================================================
// @brief: This header file defines the interface for the core functions.

#ifndef LAYER_H
#define LAYER_H

#include "typedefs.h"

template <int H, int W, int S>
void upsample(float input[H][W][3], float output[H * S][W * S][3])
{
  FOR_PIXELS(r, c, chan, H * S, W * S) {
    output[r][c][chan] = input[r / S][c / S][chan];
  }
}

template <int H, int W, int KS>
void convolve(float input[H][W][3], float output[H][W][3], const float kernel[KS][KS])
{
  for (int i = KS / 2; i < H - KS / 2; ++i) {
    for (int j = KS / 2; j < W - KS / 2; ++j) {
      float result[3] = {0.0f, 0.0f, 0.0f};
      for (int ki = -KS / 2; ki <= KS / 2; ++ki) {
        for (int kj = -KS / 2; kj <= KS / 2; ++kj) {
          result[0] += input[i + ki][j + kj][0] * kernel[ki + KS / 2][kj + KS / 2];
          result[1] += input[i + ki][j + kj][1] * kernel[ki + KS / 2][kj + KS / 2];
          result[2] += input[i + ki][j + kj][2] * kernel[ki + KS / 2][kj + KS / 2];
        }
      }
      for (int chan = 0; chan < 3; ++chan) {
        output[i][j][chan] = result[chan];
      }
    }
  }
}

// //----------------------------------------------------------
// // Padding
// //----------------------------------------------------------
// // @param[in] : input - input fmaps
// //              M - number of input fmaps
// //              I - width of input fmaps
// // @param[out] : output - output fmaps
// template <int M, int I>
// void pad(bit input[M][I][I], bit output[M][I + F_PAD][I + F_PAD]) {

//   for (int m = 0; m < M; m++) {
//     for (int x = 0; x < I; x++) {
//       for (int y = 0; y < I; y++) {
//         output[m][y + F_PAD / 2][x + F_PAD / 2] = input[m][y][x];
//       }
//     }
//   }
// }

// //----------------------------------------------------------
// // Initialize Padded Memory with Constant
// //----------------------------------------------------------
// // @param[in] : input - input fmaps to be initialized
// // @param[out] : output - output fmaps
// template <int M, int I, int C>
// void initialize_padded_memory(bit input[M][I][I]) {
//   for (int m = 0; m < M; m++) {
//     for (int x = 0; x < I; x++) {
//       for (int y = 0; y < I; y++) {
//         input[m][x][y] = C;
//       }
//     }
//   }
// }

// //----------------------------------------------------------
// // Perform Convolution Layer
// //----------------------------------------------------------
// // @param[in] : input - input fmaps
// //              threshold - threshold for batchnorm operation
// //              M - number of input fmaps
// //              N - number of output fmaps
// //              I - width of input fmaps
// //              weight - layer weights
// // @param[out] : output - output fmaps
// template <int M, int N, int I>
// void conv(bit input[M][I][I], bit output[N][I - F + 1][I - F + 1],
//           const bit8_t threshold[N], const bit weight[M][N][F][F]) {
//   int num_accum = F * F * M;
//   for (int n = 0; n < N; n++) {
//     for (int x = 0; x < I - F + 1; x++) {
//       for (int y = 0; y < I - F + 1; y++) {
//         bit16_t accum = 0;
//         for (int c = 0; c < F; c++) {
//           for (int r = 0; r < F; r++) {
//             for (int m = 0; m < M; m++) {
//               accum += input[m][y + r][x + c] == weight[m][n][r][c];
//             }
//           }
//         }
//         accum = (accum << 1) - num_accum;
//         output[n][y][x] = accum > threshold[n] ? 1 : 0;
//       }
//     }
//   }
// }

// //----------------------------------------------------------
// // Max pooling
// //----------------------------------------------------------
// // @param[in] : input - input fmaps
// //              M - number of input fmaps
// //              I - width of input fmaps
// // @param[out] : output - output fmaps
// template <int M, int I>
// void max_pool(bit input[M][I][I], bit output[M][I / 2][I / 2]) {

//   for (int m = 0; m < M; m++) {
//     for (int x = 0; x < I / 2; x++) {
//       for (int y = 0; y < I / 2; y++) {
//         bit max = 0;
//         for (int c = 0; c < 2; c++) {
//           for (int r = 0; r < 2; r++) {
//             if (input[m][2 * y + r][2 * x + c])
//               max = 1;
//           }
//         }
//         output[m][y][x] = max;
//       }
//     }
//   }
// }

// //----------------------------------------------------------
// // Flatten the Output from Conv Layer
// //----------------------------------------------------------
// // @param[in] : input - output fmaps from the last conv layer
// // @param[out] : output - input famps of the first dense layer

// void flatten(bit input[O_CHANNEL2][O_WIDTH][O_WIDTH], bit output[I_UNITS1]) {
//   for (int c = 0; c < O_CHANNEL2; c++) {
//     for (int y = 0; y < O_WIDTH; y++) {
//       for (int x = 0; x < O_WIDTH; x++) {
//         int o_index = c + (x + y * O_WIDTH) * O_CHANNEL2;
//         output[o_index] = input[c][y][x];
//       }
//     }
//   }
// }

// //----------------------------------------------------------
// // Perform Sign Layer
// //----------------------------------------------------------
// // @param[in] : input - input fmaps
// //              M - number of input and output channels
// // @param[out] : output - output fmaps

// template <int M> void sign(bit16_t input[M], bit output[M]) {
//   for (int m = 0; m < M; m++) {
//     output[m] = (input[m] > 0) ? 1 : 0;
//   }
// }

// //----------------------------------------------------------
// // Perform Argmax Layer
// //----------------------------------------------------------
// // @param[in] : input - input channels
// // @param[out] : output - argmax of the inputs

// bit4_t argmax(bit16_t input[NUM_DIGITS]) {
//   bit16_t max = input[0];
//   bit4_t max_id = 0;
//   for (int i = 1; i < NUM_DIGITS; i++) {
//     if (input[i] > max) {
//       max = input[i];
//       max_id = i;
//     }
//   }
//   return max_id;
// }

// //----------------------------------------------------------
// // Perform Dense Layer
// //----------------------------------------------------------
// // @param[in] : input - input fmaps
// //              M - number of input fmaps
// //              N - number of output fmaps
// //              weight - layer weights
// // @param[out] : output - output fmaps

// template <int M, int N>
// void dense(bit input[M], bit16_t output[N], const bit weight[M][N]) {
//   for (int n = 0; n < N; n++) {
//     bit16_t accum = 0;
//     for (int m = 0; m < M; m++) {
//       int w_index = m * N + n;
//       accum += input[m] == weight[m][n]; // XNOR
//     }
//     output[n] = (accum << 1) - M;
//   }
// }

#endif
