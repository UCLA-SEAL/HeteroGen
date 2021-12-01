

# HeteroGen-Fuzzing
This folder contains our fuzzing tool for coverage-guided test input generation.

## Setup AFL
Our tool is built on top of AFL `https://lcamtuf.coredump.cx/afl/`. Please note that you do not have to download and install afl-fuzz because it has already been integrated into our tool. We use afl-gcc/afl-g++ to instrument test programs for branch coverage monitoring.

```bash
# Build HeteroGen-Fuzzing
cd 2-test-generation && make

# Set AFL directory location
export AFL_DIR=$(pwd)/afl
```

## How to use HeteroGen-Fuzzing?
### Step 1. Instrument the test program
Go to the folder test-program, type the following command:
```bash
afl-g++ -I /path/to/Xilinx-library -std=c++11 main.cpp struct.cpp
```
This step is done when you see `[+] Instrumented X locations`.

### Step 2. Run HeteroGen-fuzz with the instrumented binary
Go back to the `2-test-generation` folder, type the following command:
```bash
./heterogen-fuzz good-seeds/ good-outputs/ 1000 ../test-program/a.out
```
You can find the generated test inputs in `good-outputs` folder. The generated inputs will be used to check behavior preservation in the iterative code edit process.
