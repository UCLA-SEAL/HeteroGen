
## Prerequisite

The compilation of test programs require a preinstalled Vivado/Vitis HLS environment and a valid Xilinx license. Please make sure they are in your `PATH` environment variable, and you can run `vivado_hls` or `vitis_hls` in your terminal. Alternatively, to use C simulation only (without C synthesis), pre-included Xilinx libraries would be working.

Please note that the Xilinx library is shipped with your Vivado installation. If you do NOT have a valid license, this library will be avaialbe upon request. Please send an email to Qian Zhang \<<zhangqian@cs.ucla.edu>\>. Please do NOT distribute any of those files.


##Command to compile

```bash
g++ -I /path/to/Xilinx-library -std=c++11 main.cpp struct.cpp

```

