
#!/bin/bash

set -eu
set -x

#BINARY_DIR_PATH=/workspace/clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-16.04
BINARY_DIR_PATH=/home/qzhang/Downloads/clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-16.04
make -j1 \
  CXX=$BINARY_DIR_PATH/bin/clang++ \
  LLVM_SRC_PATH=$BINARY_DIR_PATH \
  LLVM_BUILD_PATH=$BINARY_DIR_PATH/bin \
  LLVM_BIN_PATH=$BINARY_DIR_PATH/bin

#make LLVM_BIN_PATH=$BINARY_DIR_PATH/bin test
