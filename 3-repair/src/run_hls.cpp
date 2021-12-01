//
// Created by qzhang.
//
#include <iostream>
#include <stdlib.h>

int main(int argc , char  * argv[])
{
  printf("HeteroRepair Prototype by <zhangqian@cs.ucla.edu>\n");

  //get the synthesis message
  system("vivado_hls -f run_hls_csim.tcl");
  //   system("vivado_hls -f run_hls_csim.tcl");

  return 0;
}