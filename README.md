Checkout LLVM head, this goes in `lib/Transforms/`
Edit lib/CMakeLists.txt and add a line:

    add_subdirectory(HelloIne)

Build LLVM with cmake.

    mkdir build
    cmake ../ -DCMAKE_INSTALL_PREFIX:PATH=$RISCV

Usage:

    opt -load build/lib/HelloIne.so -debug -HelloIne < demo.bc > /dev/null

Other useful:

    clang -debug -emit-llvm demo.c -c -o demo.bc -mllvm -debug 2> output.err > output.txt
    opt -debug -ipsccp < demo.bc > /dev/null 2> output2.err
    opt -debug -sccp < demo.bc > /dev/null 2> output2.err
    llvm-dis demo.bc -o demo.ll

    opt -load build/lib/HelloIne.so -debug -HelloIne < demo.bc > demo.out.bc 2> opt.err && less opt.err
    clang -o demo-out demo.out.bc

You can generate a bitcode file using Clang or llvm-gcc, and then build a C++ code which should reconstruct the same module using the cpp backend.

    llc -march=cpp demo.bc -o demo.cpp

To see all LLVM values being created, modify the constructor in /llvm/lib/IR/Value.cpp