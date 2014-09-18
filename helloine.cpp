#include "llvm/LinkAllIR.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CallSite.h"

using namespace llvm;

namespace {
  struct HelloIne : public FunctionPass {
    static char ID;
    //HelloIne() : FunctionPass(ID) {}
    HelloIne() : FunctionPass(ID) {
      //        initializeHelloInePass(*PassRegistry::getPassRegistry());
    }

    virtual bool runOnFunction(Function &F) {
      errs() << "======\n in function: ";
      errs().write_escaped(F.getName()) << '\n';

      // lifted from LLVM's CFG.cpp
      // Look for calls by this function.
      for (Function::iterator BB = F.begin(), BBE = F.end(); BB != BBE; ++BB) {
	errs() << " |- BB " << BB->getName() << "\n";
	for (BasicBlock::iterator II = BB->begin(), IE = BB->end(); II != IE; ++II) {
	  errs() << "    |- instruction " << II->getOpcodeName() << "[" << II->getName() << "]\n";
	  CallSite CS(cast<Value>(II));
	  if (CS) {
	    const Function *Callee = CS.getCalledFunction();
	    if (!Callee)
	      // Indirect calls of intrinsics are not allowed so no need to check.
	      errs() << "    |- indirect! " << II << '\n'; //Node->addCalledFunction(CS, CallsExternalNode);
	    else if (!Callee->isIntrinsic())
	      errs() << "    |- direct!" << II << '\n';
	      //Node->addCalledFunction(CS, getOrInsertFunction(Callee));
	  }
	}
      }
      return false;
    }
  };
}

FunctionPass *llvm::createHelloInePass() {
  return new HelloIne();
}

char HelloIne::ID = 0;
//static RegisterPass<HelloIne> X("hello-ine", "Hello World Pass test", false, false);

INITIALIZE_PASS_BEGIN(HelloIne, "HelloIne", "Statically lint-checks LLVM IR",
                      false, false)
//INITIALIZE_AG_DEPENDENCY(AliasAnalysis)
INITIALIZE_PASS_END(HelloIne, "HelloIne", "Statically lint-checks LLVM IR",
                    false, false)

/*
static void registerMyPass(const PassManagerBuilder &,
                           PassManagerBase &PM) {
  PM.add(new HelloIne());
}

static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerMyPass);
*/

