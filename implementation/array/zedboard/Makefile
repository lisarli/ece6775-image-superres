#==========================================================================
# Makefile
#==========================================================================
# @brief: A makefile the compiles and runs the superres program
#
# @desc: 1. "make" or "make sw" runs software execution by default
#        2. "make fpga" invokes the HW accelerator
#        3. "make clean" cleans up the directory


INC_PATH=/usr/include/vivado_hls
CFLAGS = -I${INC_PATH} -DHLS_NO_XIL_FPO_LIB -O3

.PHONY: all sw fpga

all: sw

superres-arm: superres.cpp superres_test.cpp
	@echo "Compiling & executing superres software program on ARM ..."
	g++ ${CFLAGS} $^ -o $@

result/superres_arm_sim.txt: superres-arm
	mkdir -p result
	./$< | tee $@

sw: result/superres_arm_sim.txt
	@echo "Result saved to $@"

superres-fpga: host.cpp
	@echo "Compiling host program"
	g++ ${CFLAGS} $^ -o $@
	@echo "Make sure bitstream is loaded!"

fpga: superres-fpga
	@echo "Running superres accelerator ..."
	./$<

# Clean up the directory before submission
clean:
	@echo "Clean up output files"
	rm -rf superres-arm vivado_hls.log *.prj result out.dat *~
	rm -rf superres-fpga
