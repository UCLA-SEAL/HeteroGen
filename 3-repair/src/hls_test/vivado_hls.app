<AutoPilot:project xmlns:AutoPilot="com.autoesl.autopilot.project" projectType="C/C++" name="hls_test" top="l2norm">
    <Simulation argv="">
        <SimFlow name="csim" setup="false" optimizeCompile="true" clean="false" ldflags="" mflags=""/>
    </Simulation>
    <files>
        <file name="../../1665samples.txt" sc="0" tb="1" cflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="/home/qzhang/CLionProjects/llvm-clang-samples/hls_kernel/testbed.cpp" sc="0" tb="1" cflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="/home/qzhang/CLionProjects/llvm-clang-samples/hls_kernel/main.cpp" sc="0" tb="1" cflags=" -Wno-unknown-pragmas" blackbox="false"/>
        <file name="/home/qzhang/CLionProjects/llvm-clang-samples/hls_kernel/l2norm.cpp" sc="0" tb="false" cflags="-Iinclude/template-hls-float/include -DTHLS_FW_UINT_ON_AP_UINT=1 -std=c++11 -fexceptions" blackbox="false"/>
    </files>
    <solutions>
        <solution name="solution1" status=""/>
    </solutions>
</AutoPilot:project>

