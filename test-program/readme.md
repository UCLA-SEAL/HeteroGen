
## Prerequisite

The compilation of test programs (P2 in the submission) require a preinstalled Vivado/Vitis HLS environment and a valid Xilinx license. Please make sure they are in your `PATH` environment variable, and you can run `vivado_hls` or `vitis_hls` in your terminal. Alternatively, to use C simulation only (without C synthesis), pre-included Xilinx libraries would also be working.

The Xilinx library is shipped with your Vivado installation. If you do NOT have a valid license, this library will be avaialbe upon request. Please send an email to Qian Zhang \<<zhangqian@cs.ucla.edu>\>. Please do NOT distribute any of those files.

Please note that the Xilinx libary is required for compiling test programs and generating inputs (`2-test-generation`), but not required for code edits (`3-repair`).


##Command to compile

```bash
g++ -I /path/to/Xilinx-library -std=c++11 main.cpp struct.cpp

```

