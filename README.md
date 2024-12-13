# ece6775-image-superres

An FPGA accelerator implementation for convolution-based image superresolution on a Zedboard, which features the AMD Xilinx Zynq-7000 SoC. We explore HLS optimizations including loop unrolling, memory partitioning, and quantization, and we use dataflow optimizations to scale up our design.

### Running the Code

##### Running on Software
There are two designs in the `implementation` directory. The 20x20 accelerator with array interfaces between modules is located in `implementation/array`, and the 100x100 accelerator with stream interfaces is located in `implementation/fifo`. To run the software version of either design, run `make` from the design's `ecelinux` directory. Similarly, to run the ARM version of either design, run `make sw` from the design's `zedboard` directory. The input image to be superresolved can be modified in `data/input_image.txt`, and the corresponding output image is written to `data/output_image.txt`.

##### Running on Zedboard
To run the design on the Zedboard, run `vivado_hls -f run.tcl` from the `ecelinux` directory of the target design to synthesize the design into Verilog. Once the RTL is generated, run `source run_bitstream.sh` to generate the FPGA bitstream. The resulting bitstream is named `xillydemo.bit` and should be copied to the Zedboard. After mounting an SD card to the Zedboard using `mount /mnt/sd`, copy the bitstream to the SD card using `cp xillydemo.bit /mnt/sd` and restart the Zedboard. After the restart, run `make fpga` from the `zedboard` directory of the target design to execute the superresolution on the accelerator.

### Viewing Output
The output of the superresolution can be viewed by running `reference/utils/visualize/py`. This script takes paths to a blurry input image and the superresolved output image to display the images side-by-side.

Link to Repo: [https://github.com/lisarli/ece6775-image-superres](https://github.com/lisarli/ece6775-image-superres)