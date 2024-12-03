//=========================================================================
// superres_test.cpp
//=========================================================================
// @brief: testbench for Binarized Neural Betwork(superres) digit recongnition
// application

#include <iostream>
#include <fstream>
#include "superres.h"
#include "timer.h"

using namespace std;

// Number of test instances
const int TEST_SIZE = 1; // FIXME: change back to 100

//------------------------------------------------------------------------
// Helper function for reading images and labels
//------------------------------------------------------------------------

void read_test_image(double input_image[IMAGE_DIM][IMAGE_DIM][3]) {
  std::ifstream infile("data/fox_in.txt");
  if (infile.is_open()) {
    std::string line;
    for (int r = 0; r < IMAGE_DIM; ++r) {
      for (int c = 0; c < IMAGE_DIM; ++c) {
        std::getline(infile, line);
        std::istringstream iss(line);
        iss >> input_image[r][c][0] >> input_image[r][c][1] >> input_image[r][c][2];
      }
    }
    infile.close();
  }
}

void write_test_image(double output_image[IMAGE_DIM * SCALE_FACTOR][IMAGE_DIM * SCALE_FACTOR][3]) {
  std::ofstream outfile("data/fox_out.txt");
  for (int r = 0; r < IMAGE_DIM * SCALE_FACTOR; ++r) {
    for (int c = 0; c < IMAGE_DIM * SCALE_FACTOR; ++c) {
      outfile << std::fixed << std::setprecision(2) << output_image[r][c][0] << " " << output_image[r][c][1] << " " << output_image[r][c][2] << std::endl;
    }
  }
}

//------------------------------------------------------------------------
// superres testbench
//------------------------------------------------------------------------

int main() {
  // HLS streams for communicating with the cordic block
  hls::stream<pixel_type> superres_in;
  hls::stream<pixel_type> superres_out;

  double input_image[IMAGE_DIM][IMAGE_DIM][3];
  double output_image[IMAGE_DIM * SCALE_FACTOR][IMAGE_DIM * SCALE_FACTOR][3];

  // read test images and labels
  read_test_image(input_image);

  // Timer
  Timer timer("image superres");
  timer.start();

  for (int i = 0; i < TEST_SIZE; ++i) {
    for (int r_block = 0; r_block < IMAGE_DIM; r_block += BLOCK_SIZE) {
      for (int c_block = 0; c_block < IMAGE_DIM; c_block += BLOCK_SIZE) {
        
        for (int r = r_block; r < r_block + BLOCK_SIZE && r < IMAGE_DIM; ++r) {
          for (int c = c_block; c < c_block + BLOCK_SIZE && c < IMAGE_DIM; ++c) {
            for (int chan = 0; chan < 3; ++chan) {
              superres_in.write(input_image[r][c][chan]);
            }
          }
        }

        dut(superres_in, superres_out);

        for (int r = r_block * SCALE_FACTOR; r < (r_block + BLOCK_SIZE) * SCALE_FACTOR && r < IMAGE_DIM * SCALE_FACTOR; ++r) {
          for (int c = c_block * SCALE_FACTOR; c < (c_block + BLOCK_SIZE) * SCALE_FACTOR && c < IMAGE_DIM * SCALE_FACTOR; ++c) {
            for (int chan = 0; chan < 3; ++chan) {
              output_image[r][c][chan] = superres_out.read();
            }
          }
        }
      }
    }
  }

  timer.stop();

  write_test_image(output_image);
  return 0;
}
