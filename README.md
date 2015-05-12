Installation
------------------------------------------------------------------
Checkout LLVM 3.4, this goes in `lib/Transforms/`

patch LLVM 3.4 `/lib/Transforms/IPO/PassManagerBuilder.cpp`: 

      MPM.add(createIPSCCPPass());              // IP SCCP
      MPM.add(createDeadArgEliminationPass());  // Dead argument elimination

      MPM.add(createInstructionCombiningPass());// Clean up after IPCP & DAE
      MPM.add(createCFGSimplificationPass());   // Clean up after IPCP & DAE
    }

    +// BACKPORTED PATCH
    +// http://lists.cs.uiuc.edu/pipermail/llvm-commits/Week-of-Mon-20121224/160307.html
    +// Add extensions which are enabled at optimisation level 0 or greater.  This
    +// is "duplicated" since for OptLevel > 0 the passes registered as
    +// EP_EnabledOnOptLevel0 are not inserted into the appropriate Pass Manager.
    +addExtensionsToPM(EP_EnabledOnOptLevel0, MPM);

    // Start of CallGraph SCC passes.
    if (!DisableUnitAtATime)
      MPM.add(createPruneEHPass());             // Remove dead EH info
    if (Inliner) {
      MPM.add(Inliner);
      Inliner = 0;
    }


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

Simplest: 

    PATH=/home/ubuntu/riscv-tools/riscv-isa-sim/test/single-file-tests/env/:$PATH
    riscv-clang <yourfile>
    SPIKE=spike-fptr-tags ../../../linux/run_in_spike_linux.sh $PWD/a.out | tee test.txt


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


Building nginx


./configure --without-pcre --without-http_rewrite_module --without-http_gzip_module

edit objs/Makefile

    CC =    riscv-clang
    CFLAGS = -O0

compile:
    
    PATH=/home/ubuntu/riscv-tools/riscv-isa-sim/test/single-file-tests/env/:$PATH make | tee nginx-build-log.txt

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
