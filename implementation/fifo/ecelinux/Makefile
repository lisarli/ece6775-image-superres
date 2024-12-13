#==========================================================================
# Makefile
#==========================================================================
# @brief: A makefile the compiles and synthesizes the superres program
#
# @desc: 1. "make" runs csim by default
#        2. "make csim" compiles & executes the software implementation
#        3. "make clean" cleans up the directory


# Extract Vivado HLS include path
XILINX_VIVADO?=/opt/xilinx/Vivado/2019.2
XIL_HLS=source $(XILINX_VIVADO)/settings64.sh; vivado_hls
VHLS_INC=$(XILINX_VIVADO)/include
# Specify compilation flags
CFLAGS=-g -I${VHLS_INC} -DHLS_NO_XIL_FPO_LIB -std=c++11 -O3

TCL_SCRIPT=run.tcl

.PHONY: all csim bitstream clean

all: csim

superres: superres.cpp superres_test.cpp superres.h layer.h typedefs.h
	g++ ${CFLAGS} $^ -o $@ -lrt

result/superres_csim.txt: superres
	@echo "Running superres sim..."
	mkdir -p result
	./$< | tee $@

csim: result/superres_csim.txt
	@echo "Result recorded to $<"

xillydemo.bit:
	@echo "================================================================="
	@echo "Synthesizing superres and creating bitstream with $(TCL_SCRIPT)..."
	@echo "================================================================="
	$(XIL_HLS) -f $(TCL_SCRIPT)
	source $(XILINX_VIVADO)/settings64.sh; ./run_bitstream.sh

bitstream: xillydemo.bit
	@echo "Bitstream saved to $<"

clean:
	rm -rf superres *.dat *.prj *.log
	rm -rf zedboard_project* xillydemo.bit
