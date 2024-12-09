#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <assert.h>

#include <iostream>
#include <fstream>

#include "timer.h"
#include "superres.h"

//------------------------------------------------------------------------
// Helper function for reading images and labels
//------------------------------------------------------------------------
const int TEST_SIZE = 1; // number of test instances
const int REPS = 20; // run over the 100 test instances 20 times to saturate the accelerator

void read_test_image(double input_image[ORIG_DIM][ORIG_DIM][3]) {
  std::ifstream infile("data/input_image.txt");
  if (infile.is_open()) {
    std::string line;
    for (int r = 0; r < ORIG_DIM; ++r) {
      for (int c = 0; c < ORIG_DIM; ++c) {
        std::getline(infile, line);
        std::istringstream iss(line);
        iss >> input_image[r][c][0] >> input_image[r][c][1] >> input_image[r][c][2];
      }
    }
    infile.close();
  }
}


void write_test_image(double output_image[OUT_DIM][OUT_DIM][3]) {
  std::ofstream outfile("data/output_image.txt");
  for (int r = 0; r < OUT_DIM; ++r) {
    for (int c = 0; c < OUT_DIM; ++c) {
      outfile << std::fixed << std::setprecision(2) << output_image[r][c][0] << " " << output_image[r][c][1] << " " << output_image[r][c][2] << std::endl;
    }
  }
}


//--------------------------------------
// main function
//--------------------------------------
int main(int argc, char **argv) {
  // Open channels to the FPGA board.
  // These channels appear as files to the Linux OS
  int fdr = open("/dev/xillybus_read_32", O_RDONLY);
  int fdw = open("/dev/xillybus_write_32", O_WRONLY);

  // Check that the channels are correctly opened
  if ((fdr < 0) || (fdw < 0)) {
    fprintf(stderr, "Failed to open Xillybus device channels\n");
    exit(-1);
  }

  // Arrays to store input and output images
  double input_image[ORIG_DIM][ORIG_DIM][3];
  double output_image[OUT_DIM][OUT_DIM][3];

  //--------------------------------------------------------------------
  // Read test image into array
  //--------------------------------------------------------------------
  read_test_image(input_image);

  // Timer
  Timer timer("image superres");
  // intermediate results
  int nbytes;
  int error = 0;
  int num_test_insts = 0;
  float correct = 0.0;


  //--------------------------------------------------------------------
  // Run it once without timer to test accuracy
  //--------------------------------------------------------------------
  std::cout << "Running Image Superres on Zedboard" << std::endl;

  pixel_type pixel;
  bit32_t bits_out;

  // Send data to accelerator
  for (int i = 0; i < TEST_SIZE; ++i) {
    FOR_PIXELS(r, c, chan, ORIG_DIM, ORIG_DIM) {
      pixel = input_image[r][c][chan];
      bits_out = pixel(31,0);
      nbytes = write(fdw, (void *)&bits_out, sizeof(bits_out));
      assert(nbytes == sizeof(bits_out));
    }
  }

  std::cout << "sent to accel" << std::endl;

  // Receive data from accelerator
  for (int i = 0; i < TEST_SIZE; ++i) {
    FOR_PIXELS(r, c, chan, OUT_DIM, OUT_DIM) {
      std::cout << "waiting for pixel: " << r << c << chan << std::endl;
      nbytes = read(fdr, (void *)&bits_out, sizeof(bits_out));
      assert(nbytes == sizeof(bits_out));

      pixel(31,0) = bits_out;
      std::cout << "\tgot: " << pixel << std::endl;
      output_image[r][c][chan] = pixel;
    }
  }
  write_test_image(output_image);
/*
  //--------------------------------------------------------------------
  // Run it 20 times to test performance
  //--------------------------------------------------------------------
  std::cout << "Testing performance over " << REPS*TEST_SIZE << " images." << std::endl;
  timer.start();
  // Send data to accelerator
  for (int r = 0; r < REPS; r++) {
    for (int i = 0; i < TEST_SIZE; ++i) {
      FOR_PIXELS(r, c, chan, ORIG_DIM, ORIG_DIM) {
        pixel = input_image[r][c][chan];
        bits_out = pixel(31,0);
        nbytes = write(fdw, (void *)&bits_out, sizeof(bits_out));
        assert(nbytes == sizeof(bits_out));
      }
    }
  }
  // Receive data from the accelerator
  for (int r = 0; r < REPS; r++) {
    for (int i = 0; i < TEST_SIZE; ++i) {
      FOR_PIXELS(r, c, chan, OUT_DIM, OUT_DIM) {
        nbytes = read(fdr, (void *)&bits_out, sizeof(bits_out));
        assert(nbytes == sizeof(bits_out));

        pixel(31,0) = nbytes;
        output_image[r][c][chan] = pixel;
      }
    }
  }
  timer.stop();
  // total time wil be automatically printed upon exit.
*/
  return 0;
}
