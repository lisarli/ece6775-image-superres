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
const int TEST_SIZE = 100;

//------------------------------------------------------------------------
// Helper function for reading images and labels
//------------------------------------------------------------------------

void read_test_image(double input_image[ORIG_HEIGHT][ORIG_WIDTH][3]) {
  std::ifstream infile("data/input_image.txt");
  if (infile.is_open()) {
    std::string line;
    for (int r = 0; r < ORIG_HEIGHT; ++r) {
      for (int c = 0; c < ORIG_WIDTH; ++c) {
        std::getline(infile, line);
        std::istringstream iss(line);
        iss >> input_image[r][c][0] >> input_image[r][c][1] >> input_image[r][c][2];
      }
    }
    infile.close();
  }
}

void write_test_image(double output_image[ORIG_HEIGHT * SCALE_FACTOR][ORIG_WIDTH * SCALE_FACTOR][3]) {
  std::ofstream outfile("data/output_image.txt");
  for (int r = 0; r < ORIG_HEIGHT * SCALE_FACTOR; ++r) {
    for (int c = 0; c < ORIG_WIDTH * SCALE_FACTOR; ++c) {
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

  double input_image[ORIG_HEIGHT][ORIG_WIDTH][3];
  double output_image[ORIG_HEIGHT * SCALE_FACTOR][ORIG_WIDTH * SCALE_FACTOR][3];

  // read test images and labels
  read_test_image(input_image);

  // Timer
  Timer timer("image superres");
  timer.start();

  for (int i = 0; i < TEST_SIZE; ++i) {
    FOR_PIXELS(r, c, chan, ORIG_HEIGHT, ORIG_WIDTH) {
      superres_in.write(input_image[r][c][chan]);
    }

    dut(superres_in, superres_out);
    
    FOR_PIXELS(r, c, chan, ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR) {
      output_image[r][c][chan] = superres_out.read();
    }
  }

  timer.stop();

  write_test_image(output_image);
  return 0;
}
