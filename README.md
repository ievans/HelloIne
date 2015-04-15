Installation
------------------------------------------------------------------
Checkout LLVM head, this goes in `lib/Transforms/`
Edit lib/CMakeLists.txt and add a line:

    add_subdirectory(HelloIne)

If not using cmake, edit lib/Makefile and add HelloIne to the list of directories

    PARALLEL_DIRS = Utils Instrumentation Scalar InstCombine IPO Vectorize Hello HelloIne ObjCARC

Build LLVM with cmake.

    mkdir build; cd build
    cmake ../ -DCMAKE_INSTALL_PREFIX:PATH=$RISCV

or make

    mkdir build; cd build
    ../configure
    make

Usage
------------------------------------------------------------------
Verify clang on path is the riscv clang 

    which clang

Compile for target (-O1 *is* necessary due to pass registration proceedure, which only runs it if optimization is on):

    clang -O1 -target riscv -mriscv=RV64IAMFD -Xclang -load -Xclang /usr/local/lib/HelloIne.so -c demo.c -o demo.S
    $RISCV/bin/riscv64-unknown-elf-gcc -o demo.riscv demo.S

regular

    clang -target riscv -mriscv=RV64IAMFD -S ~/hello.c -o ~/hello.S
    $RISCV/bin/riscv64-unknown-elf-gcc -o ~/hello.riscv ~/hello.S

(old way that doesn't work with pass)

    clang -target riscv -mriscv=RV64IAMFD -debug ~/demo.c -S -o ~/demo.bc -mllvm -debug
    opt -load lib/HelloIne.so -debug -HelloIne < ~/demo.bc > ~/demo.out.bc
    llc -march=cpp -target riscv -mriscv=RV64IAMFD ~/demo.out.bc -o ~/demo.S
    $RSICV/bin/riscv64-unknown-elf-gcc -o ~/demo.riscv ~/demo.S

Other useful
------------------------------------------------------------------

    clang -debug -emit-llvm demo.c -c -o demo.bc -mllvm -debug 2> output.err > output.txt
    opt -debug -ipsccp < demo.bc > /dev/null 2> output2.err
    opt -debug -sccp < demo.bc > /dev/null 2> output2.err
    llvm-dis demo.bc -o demo.ll

    opt -load build/lib/HelloIne.so -debug -HelloIne < demo.bc > demo.out.bc 2> opt.err && less opt.err
    clang -o demo-out demo.out.bc

You can generate a bitcode file using Clang or llvm-gcc, and then build a C++ code which should reconstruct the same module using the cpp backend.

    llc -march=cpp demo.bc -o demo.cpp

To see all LLVM values being created, modify the constructor in /llvm/lib/IR/Value.cpp
