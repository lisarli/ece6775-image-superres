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

void read_test_image(float input_image[ORIG_HEIGHT][ORIG_WIDTH][3]) {
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

//------------------------------------------------------------------------
// superres testbench
//------------------------------------------------------------------------

int main() {
  // HLS streams for communicating with the cordic block
  hls::stream<float> superres_in;
  hls::stream<float> superres_out;

  float input_image[ORIG_HEIGHT][ORIG_WIDTH][3];
  float output_image[ORIG_HEIGHT * SCALE_FACTOR][ORIG_WIDTH * SCALE_FACTOR][3];

  // read test images and labels
  read_test_image(input_image);

  // Timer
  Timer timer("digirec superres");
  timer.start();

  for (int i = 0; i < TEST_SIZE; ++i) {
    FOR_PIXELS(r, c, chan, ORIG_HEIGHT, ORIG_WIDTH) {
      superres_in.write(input_image[r][c][chan]);
    }

    dut(superres_in, superres_out);
    
    FOR_PIXELS(r, c, chan, ORIG_HEIGHT * SCALE_FACTOR, ORIG_WIDTH * SCALE_FACTOR) {
      output_image[r][c][chan] = superres_out.read();
    }

    // TODO write output image to file
  }

  timer.stop();
  return 0;
}
