open_project bubble

set_top bubbleSort

add_files sort.cpp
add_files -tb bubble.cpp -cflags "-Wno-unknown-pragmas"
add_files -tb 0.dat -cflags "-Wno-unknown-pragmas"


open_solution "solution1"
set_part {xcvu9p-fsgd2104-3-e} -tool vivado
create_clock -period 10 -name default

csim_design -clean -O
#csynth_design
#cosim_design
#export_design -format ip_catalog
exit

