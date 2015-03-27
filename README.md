Checkout LLVM head, this goes in lib/Transforms/
Build LLVM with cmake.

Usage:

    opt -load build/lib/HelloIne.so -debug -HelloIne < demo.bc > /dev/null

Other useful:

    clang -debug -emit-llvm demo.c -c -o demo.bc -mllvm -debug 2> output.err > output.txt
    opt -debug -ipsccp < demo.bc > /dev/null 2> output2.err
    opt -debug -sccp < demo.bc > /dev/null 2> output2.err


    opt -load build/lib/HelloIne.so -debug -HelloIne < demo.bc > /dev/null 2> opt.err && more opt.err
    llvm-dis demo.bc -o demo.ll