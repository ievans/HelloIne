/*#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
    struct HelloIne : public FunctionPass {
        static char ID;
        HelloIne() : FunctionPass(ID) {}

        bool runOnFunction(Function &F) override {
            errs() << "HelloIne: ";
            errs().write_escaped(F.getName()) << '\n';
            return false;
        }
    };
}
char HelloIne::ID = 0;
static RegisterPass<HelloIne> X("HelloIne", "HelloIne World Pass", false, false);

*/

//#include "llvm/LinkAllIR.h"
//#include "llvm/LinkAllPasses.h"
//#include "llvm/PassManager.h"

#include "llvm/Transforms/Scalar.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/Utils/Local.h"



//#include "llvm/Pass.h"
//#include "llvm/IR/Function.h"
//#include "llvm/Support/raw_ostream.h"
//#include "llvm/Support/CallSite.h"

using namespace llvm;

namespace {
  struct HelloIne : public ModulePass {
      static char ID;
      //HelloIne() : FunctionPass(ID) {}
      HelloIne() : ModulePass(ID) {
          //HelloIne() : FunctionPass(ID) {
          //        initializeHelloInePass(*PassRegistry::getPassRegistry());
      }
    bool runOnModule(Module &M) override;
  };
}

static std::string LevelTab(int level) {
    std::string s = "";
    for (int i = 0; i < level; i++) {
        s += "\t";
    }
    return s;
}

static bool AddressIsTaken(const Value *GV, int level) {
    // Delete any dead constantexpr klingons.
    //GV->removeDeadConstantUsers();

    // todo: support lambda functions (they are valid in the LLVM IR). We should
    // make sure that they are considered globals and thus are being passed in to this analysis :)
    // search for lambdas in http://llvm.lyngvig.org/Articles/Mapping-High-Level-Constructs-to-LLVM-IR#16


    errs() << LevelTab(level) << "Value: " << GV << "\n";

    for (const Use &U : GV->uses()) {
        const User *UR = U.getUser();
        
        // User -> {Constant, Operator}
        // we pretty much only care about constants

        if (isa<GlobalObject>(UR)) {
            errs() << LevelTab(level) << "global object identified to: " << dyn_cast<GlobalObject>(UR) << "\n";
        }

        if (isa<Function>(UR)) {
            errs() << LevelTab(level) << "FUNCTION POINTER identified to: " << dyn_cast<Function>(UR)->getName() << "\n";
        }

        if (isa<Instruction>(UR)) {
            // we will insert an instruction blessing immediate of this
            // instruction with a code pointer tag. this will probably require
            // refractoring this constant into a new constant. we can use that replaceusesof thing...

            errs() << LevelTab(level) << "\tinstruction: ";
            dyn_cast<Instruction>(UR)->print(errs());
            errs() << LevelTab(level) << "\n" << LevelTab(level) << "\t[from " << dyn_cast<Instruction>(UR)->getParent()->getParent() << "]";
            errs() << LevelTab(level) << "\n";
        }

        if (const StoreInst *SI = dyn_cast<StoreInst>(UR)) {
            errs() << LevelTab(level) << "--> possible store at " << SI << "\n";
            if (SI->getOperand(0) == GV || SI->isVolatile()) {
                //return true;  // Storing addr of GV.
            }
        } else if (isa<InvokeInst>(UR) || isa<CallInst>(UR)) {
            errs() << LevelTab(level) << "--> indirect? invokation at " << UR << "\n";
            // Make sure we are calling the function, not passing the address.
            ImmutableCallSite CS(cast<Instruction>(UR));
            if (!CS.isCallee(&U)) {
                //return true;
            }
        } else if (const LoadInst *LI = dyn_cast<LoadInst>(UR)) {
            errs() << LevelTab(level) << "--> possible load at " << LI << "\n";
            if (LI->isVolatile()) {
                //return true;
            }
        } else if (isa<BlockAddress>(UR)) {
            // blockaddress doesn't take the address of the function, it takes addr
            // of label.
            errs() << LevelTab(level) << "--> block address\n";
        } else {
            errs() << LevelTab(level) << "--> UNKNOWN invokation " << U << "\n";
        }
        AddressIsTaken(UR, level + 1);
    }
    return false;
}

bool HelloIne::runOnModule(Module &M) { 
    //
    errs() << "********************************************************************************\n";
    for (Module::iterator F = M.begin(), E = M.end(); F != E; ++F) {
        if (F->isDeclaration())
            continue;

        if (AddressIsTaken(F, 0)) {
            // hi
        }
    }

    errs() << "********************************************************************************\n";
  // Loop over global variables.  We inform the solver about any internal global
  // variables that do not have their 'addresses taken'.  If they don't have
  // their addresses taken, we can propagate constants through them.
  for (Module::global_iterator G = M.global_begin(), E = M.global_end();
       G != E; ++G)
      AddressIsTaken(G, 0);
//    if (!G->isConstant() && G->hasLocalLinkage() && !AddressIsTaken(G))
//      Solver.TrackValueOfGlobalVariable(G);

  return false;
}

/*virtual bool runOnFunction(Function &F) {
      // err now walk the CFG...where?

      errs() << "======\n in function: ";
      errs().write_escaped(F.getName()) << '\n';

      // lifted from LLVM's CFG.cpp
      // Look for calls by this function.
      for (Function::iterator BB = F.begin(), BBE = F.end(); BB != BBE; ++BB) {
	errs() << " |- BB " << BB->getName() << "\n";
	for (BasicBlock::iterator II = BB->begin(), IE = BB->end(); II != IE; ++II) {
	  errs() << "    |- instruction " << II->getOpcodeName() << "[" << II->getName() << "]\n";

	  // where did it come from exactly?

	  // is it an invoke instruction--see difference:
	  // http://llvm.1065342.n5.nabble.com/callinst-vs-invokeinst-td6515.html
	  if (isa<InvokeInst>(II)) {
	    InvokeInst Invoke(cast<InvokeInst>(II));
	    // has .doesNotReturn() also
	    // is it direct or indirect?
	    const Function *Callee = Invoke.getCalledFunction();
	    if (!Callee) {
              errs() << "    |- invoke indirect! " << II << '\n';
	    } else {
              errs() << "    |- invoke direct! " << II << '\n';
	    }
	  }

	  // is it an unwind instruction? is that even relevant?
	  // see LLVM paper 
	  // http://llvm.org/pubs/2004-01-30-CGO-LLVM.pdf
	  // section 2.4

	  // is it an indirect branch?
	  // see http://llvm.org/docs/ProgrammersManual.html#the-isa-cast-and-dyn-cast-templates for documentation on "isa"
	  // note that indirect branches *only* every branch to labels--so this is not really an issue
	  // see more details on when indirect branches are generated:
	  //   http://blog.llvm.org/2010/01/address-of-label-and-indirect-branches.html
	  if (isa<IndirectBrInst>(II)) {
	    errs() << "indirect BRANCH " << II << '\n';
	  }

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
*/

//FunctionPass *llvm::createHelloInePass() { 
//  return new HelloIne();
//}

//char HelloIne::ID = 0;
//static RegisterPass<HelloIne> X("hello-ine", "Hello World Pass test", false, false);

char HelloIne::ID = 0;
static RegisterPass<HelloIne> X("HelloIne", "HelloIne World Pass", false, false);
/*
INITIALIZE_PASS_BEGIN(HelloIne, "HelloIne", "Statically lint-checks LLVM IR",
                      false, false)
//INITIALIZE_AG_DEPENDENCY(AliasAnalysis)
INITIALIZE_PASS_END(HelloIne, "HelloIne", "Statically lint-checks LLVM IR",
                    false, false)
*/
/*
static void registerMyPass(const PassManagerBuilder &,
                           PassManagerBase &PM) {
  PM.add(new HelloIne());
}

static RegisterStandardPasses
RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible, registerMyPass);
*/

