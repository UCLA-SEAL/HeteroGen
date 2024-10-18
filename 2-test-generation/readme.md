

# HeteroGen-fuzz
This folder contains our fuzzing tool for coverage-guided test input generation.

## Prerequisite
This test generation tool is developed using Ubuntu 18.04 with gcc 7.5.0.

## How to install this fuzzing tool?
Our tool is built on top of AFL `https://lcamtuf.coredump.cx/afl/`. Please note that you do not have to download and install afl-fuzz because it has already been integrated into our tool. We use afl-gcc/afl-g++ to instrument test programs for branch coverage monitoring.

```bash
# Build HeteroGen-fuzz
cd 2-test-generation && make

# Set AFL directory location
export AFL_DIR=$(pwd)/2-test-generation
```

## How to use HeteroGen-fuzz?
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
Make sure output folder exists before using heterogen-fuzz.

You can find the generated test inputs in `good-outputs` folder. The generated inputs will be used to check behavior preservation in the automated code edit process. This step generates the results presented in Table 4. Please note that fuzzing is a random test generation process; thus, the results vary significantly across different runs.
