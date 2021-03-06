

# HeteroGen-repair
This folder contains our code edit tool for error removal.

## Prerequisite
- Ubuntu 18.04
- gcc 7.5.0
- clang+llvm-6.0.0
- Vivado/Vitis HLS installed (optional, we have removed the hls invocation in this artifact to simplify the setup and evaluation process.)

## How to install this tool?
Please follow the instructions below.

```bash
# Build llvm dependence
Please download and install the llvm version 6.0.0 `https://releases.llvm.org/6.0.0/` for your specific PC platform. 
After setting up your llvm environment, please update the `path` in our script `build_vs_released_binary.sh`.

# Install
cd 3-repair && ./build_vs_released_binary.sh
```

## How to use HeteroGen-repair?
Please note that the repair tool reads the logs generated by HLS tool. Here we have the log file ready for our test program P2. The usage of HeteroGen-repair follows: repair `filename` `logname`

Go to the folder `3-repair/build`, type the following command:
```bash
./repair struct.cpp solution1.log
or ./repair struct.cpp solution.log > result.txt
```
This step prints out the transformed code (Table 3) based on errors in the log file. Please ignore the "'hls_stream.h' file not found" error if you do not have HLS libraries. If you use the first command, you will see the results in your terminal; otherwise, please find the result.txt file in the same folder.
