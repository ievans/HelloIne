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


//for functionlisttype
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ADT/ValueMap.h"
//#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "llvm/Transforms/Scalar.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/PointerIntPair.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/ConstantFolding.h"
//#include "llvm/Analysis/TargetLibraryInfo.h"

//#include "llvm/IR/CallSite.h"
#include "llvm/Support/CallSite.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
//#include "llvm/IR/DerivedTypes.h"
//#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/Instructions.h"
//#include "llvm/Pass.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
//#include "llvm/Transforms/IPO.h"
//#include "llvm/Transforms/Utils/Local.h"

#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/ValueSymbolTable.h"

//#include "llvm/Pass.h"
//#include "llvm/IR/Function.h"
//#include "llvm/Support/raw_ostream.h"
//#include "llvm/Support/CallSite.h"

using namespace llvm;

namespace {
  class HelloIne : public ModulePass {
  public:
      static char ID;
      //HelloIne() : FunctionPass(ID) {}
      HelloIne() : ModulePass(ID) {
          //HelloIne() : FunctionPass(ID) {
          //        initializeHelloInePass(*PassRegistry::getPassRegistry());
      }
      virtual bool runOnModule(Module &M); //override;
      bool IsCodePointer(Value *GV, llvm::LLVMContext& context, int level);
    //bool shouldProtectType(Type *Ty, bool IsStore, MDNode *TBAATag);
    //    DenseMap<StructType*, MDNode*> StructsTBAA;
    //    DenseMap<StructType*, MDNode*> UnionsTBAA;
   // map of {function -> global values used in that function}
   ValueMap<Function*, SmallVector<Value*, 16> > UserFunctionMap;
  };
}


static std::string LevelTab(int level) {
    std::string s = "";
    for (int i = 0; i < level; i++) {
        s += "\t";
    }
    return s;
}


/*
static MDNode *getNextElTBAATag(size_t &STBAAIndex, Type *ElTy,
                                const StructLayout *SL, unsigned idx,
                                MDNode *STBAATag) {
    if (ElTy->isSingleValueType() && STBAATag) {
        size_t Off = SL->getElementOffset(idx);
        size_t STBAASize = STBAATag->getNumOperands();

        // skip over embedded structs (if any)
        while (STBAAIndex+2 < STBAASize &&
               cast<ConstantInt>(STBAATag->getOperand(STBAAIndex))
               ->getValue().ult(Off)) STBAAIndex += 3;

        if (STBAAIndex+2 < STBAASize &&
            cast<ConstantInt>(STBAATag->getOperand(STBAAIndex))
            ->equalsInt(Off)) {
            // The struct type might be union, in which case we'll have >1 tags
            // for the same offset.
            if (STBAAIndex+3+2 < STBAASize &&
                cast<ConstantInt>(STBAATag->getOperand(STBAAIndex+3))
                ->equalsInt(Off)) {
                // FIXME: support unions
            } else {
                //FIXME: the following assertion seems to not hold for bitfields
                //assert(cast<ConstantInt>(STBAATag->getOperand(STBAAIndex+1))
                //       ->equalsInt(DL->getTypeAllocSize(ElTy)));
                return cast<MDNode>(STBAATag->getOperand(STBAAIndex+2));
            }
        }
    }

    return NULL;
}

*/

// bool HelloIne::shouldProtectType(Type *Ty, bool IsStore, MDNode *TBAATag) {
//     if (Ty->isFunctionTy() ||
//         (Ty->isPointerTy() &&
//          cast<PointerType>(Ty)->getElementType()->isFunctionTy())) {
//         return true;

//     } else if (/*Ty->isPrimitiveType() || */ Ty->isIntegerTy()) {
//         return false;

//     } else if (PointerType *PTy = dyn_cast<PointerType>(Ty)) {
//         // FIXME: for unknown reason, clang sometimes generates function pointer
//         // items in structs as {}* (e.g., in struct _citrus_iconv_ops). However,
//         // clang keeps correct TBAA tags even in such cases, so we look at it first.
//         if (IsStore && PTy->getElementType()->isStructTy() &&
//             cast<StructType>(PTy->getElementType())->getNumElements() == 0 &&
//             TBAATag && TBAATag->getNumOperands() > 1 &&
//             cast<MDString>(TBAATag->getOperand(0))->getString() ==
//             "function pointer") {
//             return true;
//         }

//         if (true) { //CPSOnly) {
//             Type *ElTy = PTy->getElementType();
//             if (ElTy->isPointerTy()) {
//                 //     cast<PointerType>(ElTy)->getElementType()->isFunctionTy()) {
//                 // It could be a vtable pointer
//                 if (TBAATag) {
//                     assert(TBAATag->getNumOperands() > 1);
//                     MDString *TagName = cast<MDString>(TBAATag->getOperand(0));
//                     return TagName->getString() == "vtable pointer";
//                 }
//             }

//             if (IsStore && ElTy->isIntegerTy(8)) {
//                 // We want to instrument all stores of void* pointers, as those
//                 // might later be casted to protected pointers. Unfortunately,
//                 // LLVM represents all void* pointers as i8*, so we do something
//                 // very over-approximate here.

//                 if (TBAATag) {
//                     assert(TBAATag->getNumOperands() > 1);
//                     MDString *TagName = cast<MDString>(TBAATag->getOperand(0));
//                     return TagName->getString() == "void pointer" ||
//                         TagName->getString() == "function pointer";
//                 }
//             }

//             return false;
//         }

//         if (IsStore && PTy->getElementType()->isIntegerTy(8)) {
//             // We want to instrument all stores of void* pointers, as those
//             // might later be casted to protected pointers. Unfortunately,
//             // LLVM represents all void* pointers as i8*, so we do something
//             // very over-approximate here.

//             if (TBAATag) {
//                 assert(TBAATag->getNumOperands() > 1);
//                 MDString *TagName = cast<MDString>(TBAATag->getOperand(0));
//                 return TagName->getString() == "void pointer" ||
//                     TagName->getString() == "function pointer";
//             }

//             return true;
//         }

//         return shouldProtectType(PTy->getElementType(), IsStore, NULL);

//     } else if (SequentialType *PTy = dyn_cast<SequentialType>(Ty)) {
//         return shouldProtectType(PTy->getElementType(), IsStore, NULL);

//     } else if (StructType *STy = dyn_cast<StructType>(Ty)) {
//         if (STy->isOpaque())
//             return IsStore;

//         //TypesProtectInfoTy::key_type Key(Ty, IsStore);
//         //TypesProtectInfoTy::iterator TIt = StructTypesProtectInfo.find(Key);
//         //if (TIt != StructTypesProtectInfo.end())
//         //return TIt->second;

//         // Avoid potential infinite recursion due to recursive types
//         // FIXME: support recursive types with sensitive members
//         //StructTypesProtectInfo[Key] = false;

//         if (MDNode *UTBAATag = UnionsTBAA.lookup(STy)) {
//             // This is a union, try casting it to all components
//             for (unsigned i = 0, e = UTBAATag->getNumOperands(); i+1 < e; i += 2) {
//                 //assert(isa<UndefValue>(UTBAATag->getOperand(i)));
//                 //assert(isa<MDNode>(UTBAATag->getOperand(i+1)));

//                 /*
// TOOD
//                 Type *ElTy = UTBAATag->getOperand(i)->getType();
//                 MDNode *ElTBAATag = cast<MDNode>(UTBAATag->getOperand(i+1));
//                 if (shouldProtectType(ElTy, IsStore, ElTBAATag)) {
//                     //StructTypesProtectInfo[Key] = true;
//                     return true;
//                 }
//                 */
//             }


//             return false;
//         } else {
//             // Tnis is not a union, go through all fields
//             MDNode *STBAATag = StructsTBAA.lookup(STy);
//             if (!STBAATag) {
//                     dbgs() << "CPI: missing struct TBAA for ";
//                     if (STy->hasName()) dbgs() << STy->getName();
//                     dbgs() << "\n    "; STy->dump();
//                     dbgs() << "\n";
//             }

// /*
//             const StructLayout *SL = STBAATag ? DL->getStructLayout(STy) : NULL;
//             size_t STBAAIndex = 0;

//             for (unsigned i = 0, e = STy->getNumElements(); i != e; ++i) {
//                 Type *ElTy = STy->getElementType(i);
//                 MDNode *ElTBAATag =
//                     getNextElTBAATag(STBAAIndex, ElTy, SL, i, STBAATag);

//                 if (shouldProtectType(ElTy, IsStore, ElTBAATag)) {
//                     // Cache the results to speedup future queries
//                     //StructTypesProtectInfo[Key] = true;
//                     return true;
//                 }
//             }
// */
//             return false;
//         }

//     } else {
//         Ty->dump();
//         llvm_unreachable("Unhandled type");
//     }
// }


static void ProcessInstruction(Value *GV, Value::use_iterator U, User* UR, llvm::LLVMContext& context, int level) {
      Instruction* existingInst = dyn_cast<Instruction>(UR);

      // we will insert an instruction blessing immediate of this
      // instruction with a code pointer tag. this will probably require
      // refractoring this constant into a new constant. we can use that replaceusesof thing...

      // insert new instruction
      //Instruction* newInst = new Instruction( );
      //IRB.CreateAdd(Base, IRB.CreateIntCast(Size, IntPtrTy, false))
      //existingInst->getParent()->getInstList().insert(existingInst, newInst);
      errs() << LevelTab(level) << "\tinstruction: ";
      // dyn_cast<Instruction>(UR)->print(errs());
      // errs() << LevelTab(level) << "\tdebug location: ";
      // dyn_cast<Instruction>(UR)->getDebugLoc().print(errs());
      // errs() << LevelTab(level) << "\n" << LevelTab(level) << "\t[from " << dyn_cast<Instruction>(UR)->getParent()->getParent() << "]";
      // errs() << LevelTab(level) << "\n";

      if (isa<CallInst>(existingInst)) {
	if (cast<CallInst>(existingInst)->isInlineAsm()) {
	  errs() << "skipped a call instruction because INLINE ASM\n";
          return;
	}
	if (cast<CallInst>(existingInst)->getCalledFunction()) {
	  // the called function exists--it is a direct call; no need
	  // to instrument it IFF the callee is this USE (b/c the
	  // arguments might be function pointers)

	  // TODO reenable this
	  // Make sure we are calling the function, not passing the address.
	  CallSite CS(*U); //cast<Instruction>(U));
	  if (CS.isCallee(U)) {
	       errs() << "skipped a call instruction because call address immediate\n";
	       return;
	  }
	}
      }
      if (const StoreInst *SI = dyn_cast<StoreInst>(existingInst)) {
	if (SI->isVolatile()) {
	  errs() << "skipped a store instruction because volatile and therefor inserted by us (TODO fix this)\n";
	  return;
	}
      }
        
      // errs() << " creating blessed storage\n";
      // AllocaInst* blessed_storage = new AllocaInst(GV->getType(), "blessed_use", existingInst);
      // errs() << " creating blessed STORE\n";
      // StoreInst* blessed_store = new StoreInst(GV, blessed_storage, /* volatile = */ true, existingInst);
      // errs() << " creating blessed LOAD\n";
      // LoadInst* blessed_load = new LoadInst(blessed_storage, "blessed load", /* volatile = */ true, existingInst);
      // blessed_load->setAlignment(4);


      /********************************************************************************/
      /* inline asm */

      std::vector<Type*>AsmFuncTy_args;
      AsmFuncTy_args.push_back(GV->getType());
      FunctionType* AsmFuncTy = FunctionType::get(
          /*Result=*/GV->getType(), //IntegerType::get(mod->getContext(), 32), //Type::getVoidTy(context),
          /*Params=*/AsmFuncTy_args,
          /*isVarArg=*/false);

      // n.b.; gcc uses %0 but clang uses $0 to refer to the operand
      // "2" is the tag we are setting on the thing
      std::string riscv_set = "settag $0, 2";
      std::string x86_set = "addi $0, $0, 1337"; //int $$0x1337";
      InlineAsm* myinlineasm = InlineAsm::get(AsmFuncTy, riscv_set, "=r,r,~{dirflag},~{fpsr},~{flags}",true);
      //std::vector<Value*> asm_params;
      //asm_params.push_back(blessed_load);
      errs() << " creating bless-ing call\n";
      CallInst* asm_call = CallInst::Create(myinlineasm, GV, "", existingInst);
      //asm_call->setCallingConv(CallingConv::C);
      asm_call->setTailCall(false);
      AttributeSet asm_call_PAL;
      {
	SmallVector<AttributeSet, 4> Attrs;
	AttributeSet PAS;
	{
	  AttrBuilder B;
	  B.addAttribute(Attribute::NoUnwind);
	  PAS = AttributeSet::get(context, ~0U, B);
	}
	Attrs.push_back(PAS);
	asm_call_PAL = AttributeSet::get(context, Attrs);
      }
      asm_call->setAttributes(asm_call_PAL);

      errs() << " REPLACING\n";
      //GV->replaceAllUsesWith(blessed_load);
      //existingInst->replaceUsesOfWith(GV, blessed_store->getValueOperand());
      //existingInst->replaceUsesOfWith(GV, blessed_load);
      existingInst->replaceUsesOfWith(GV, asm_call);
      errs() << "replace worked!!\n";

      /********************************************************************************/

      //blessed_store->setAlignment(8);
      //BitCastInst *TheBC = new BitCastInst(blessed_storage, GV->getType(), "newgv", existingInst);
}


// fix the globals
//void AddressSanitizerModule::createInitializerPoisonCalls(
/*
void createInitializerTags(Module &M, GlobalValue *ModuleName) {
    GlobalVariable *GV = M.getGlobalVariable("llvm.global_ctors");
    ConstantArray *CA = cast<ConstantArray>(GV->getInitializer());
    for (Use &OP : CA->operands()) {
        if (isa<ConstantAggregateZero>(OP)) continue;
        ConstantStruct *CS = cast<ConstantStruct>(OP);

        // Must have a function or null ptr.
        if (Function *F = dyn_cast<Function>(CS->getOperand(1))) {
            if (F->getName() == kAsanModuleCtorName) continue;
            ConstantInt *Priority = dyn_cast<ConstantInt>(CS->getOperand(0));
            // Don't instrument CTORs that will run before asan.module_ctor.
            if (Priority->getLimitedValue() <= kAsanCtorAndDtorPriority) continue;
            
            // TODO
            //poisonOneInitializer(*F, ModuleName);
        }
    }
}
*/

//insert uses in front of fction pointers 
void poisonOneInitializer(Function &GlobalInit, GlobalValue *ModuleName) {
    // Set up the arguments to our poison/unpoison functions.
    IRBuilder<> IRB(GlobalInit.begin()->getFirstInsertionPt());
    // Add a call to poison the ues external globals before the given function starts.
    //Value *ModuleNameAddr = ConstantExpr::getPointerCast(ModuleName, IntptrTy);
    //IRB.CreateCall(AsanPoisonGlobals, ModuleNameAddr);
    
    
}

Function* findEnclosingFunc(Value* V) {
    if (Argument* Arg = dyn_cast<Argument>(V)) {
        return Arg->getParent();
    }
    if (Instruction* I = dyn_cast<Instruction>(V)) {
        return I->getParent()->getParent();
    }
    return NULL;
}



bool HelloIne::IsCodePointer(Value *GV, llvm::LLVMContext& context, int level) {
  // Delete any dead constantexpr klingons.
  //GV->removeDeadConstantUsers();

  // todo: support lambda functions (they are valid in the LLVM IR). We should
  // make sure that they are considered globals and thus are being passed in to this analysis :)
  // search for lambdas in http://llvm.lyngvig.org/Articles/Mapping-High-Level-Constructs-to-LLVM-IR#16
    if (level >= 10) { errs() << " recursion depth exceeded\n"; return true; } 


  bool isCodePointer = false;
  errs() << LevelTab(level) << "Value: " << GV << "(name: " << GV->getName() << "\n";
  GV->getType()->print(errs());
  errs() << "llvm is fuction type" << GV->getType()->isFunctionTy() << "\n";
  //errs() << "linkage: " << isa<BasicBlock>(GV); // should be true if it is a jump target address
  //errs() << "linkage: " << isa<MetadataAsValue>(GV); // dunno what this is

  //use_iterator
  //for (Use &U : GV->uses()) {

  for (Value::use_iterator U = GV->use_begin(), e = GV->use_end(); U != e; ++U) {
    //const Instruction *User = cast<Instruction>(*UI);
    User *UR = *U; //U.getUser();
    
    // is the use a direct call? if so, we don't need to protect
    if (isa<CallInst>(UR)) {
	if (cast<CallInst>(UR)->isInlineAsm()) {
            errs() << "skipped a call instruction because INLINE ASM\n";
            continue;
	}
	if (cast<CallInst>(UR)->getCalledFunction()) {
            // the called function exists--it is a direct call; no need
            // to instrument it IFF the callee is this USE (b/c the
            // arguments might be function pointers)

            // TODO reenable this
            // Make sure we are calling the function, not passing the address.
            CallSite CS(*U); //cast<Instruction>(U));
            if (CS.isCallee(U)) {
                errs() << "skipped a call instruction because call address immediate\n";
                continue;
            }
	}
    }


    // if (const StoreInst *SI = dyn_cast<StoreInst>(UR)) {
    //     errs() << "--> possible store at " << SI << "\n";
    //     if (SI->getOperand(0) == GV || SI->isVolatile()) {
    //         //return true;  // Storing addr of GV.
    //     } else { continue; }
    // } 


    // User -> {Constant, Operator}
    // what function is this in?
    Function* f = findEnclosingFunc(UR);
    if (UserFunctionMap.count(f)) {
        UserFunctionMap[f].push_back(GV);
    } else {
        SmallVector<Value *, 16> vect;
        vect.push_back(GV);
        UserFunctionMap[f] = vect;
    }
  }

/*
    if (isa<Instruction>(UR)) {
        ProcessInstruction(GV, U, UR, context, level);
    } else {
      errs() << "non instruction use!!\n";
      //if (isa<GlobalObject>(UR)) { errs() << "global object\n"; }
      if (isa<GlobalVariable>(GV)) { errs() << "global variable\n"; }
      if (isa<ConstantExpr>(UR)) { errs() << "constant expr\n"; }
      if (isa<Constant>(UR)) { errs() << "constant nonexpr\n"; } 

// TODO: doesnt work in llvm 34 
      if (isa<GlobalVariable>(UR) && cast<GlobalVariable>(UR)->hasInitializer()) {
          errs() << "has initializer!!\n";
          if (!cast<GlobalVariable>(UR)->getInitializer()) {
              errs() << "got! initializer!!\n";
          }
      }
      // TODO: technically we should be looking for the initializer and blessing
      // their, rather than at use--this violates our security guarantees
      //errs() << "doing recursive call\n";
      //IsCodePointer(UR, context, level + 1);
    }
  }
*/

/*  
     else if (isa<InvokeInst>(UR) || isa<CallInst>(UR)) {
      // errs() << LevelTab(level) << "--> indirect? invokation at " << UR << "\n";
      // isCodePointer = true;

      // //Function* llvm::CallInst::getCalledFunction
      // //getCalledFunction - Return the function called, or null if this is an indirect function invocation.


      // // Make sure we are calling the function, not passing the address.
      // ImmutableCallSite CS(cast<Instruction>(UR));
      // if (!CS.isCallee(&U)) {
      //     //return true;
      // }
    } else if (const LoadInst *LI = dyn_cast<LoadInst>(UR)) {
      //            errs() << LevelTab(level) << "--> possible load at " << LI << "\n";
      // examine operands--any of them functions?
      if (LI->isVolatile()) {
	//return true;
      }
    } else if (isa<BlockAddress>(UR)) {
      // blockaddress doesn't take the address of the function, it takes addr
      // of label.
        errs() << "--> block address\n";
    } else {
        errs() << "--> UNKNOWN invokation " << UR << "\n";
    }
*/
    //isCodePointer =  isCodePointer || IsCodePointer(UR, context, level + 1);
  return isCodePointer;
}

// void EnumerateValue(Value* v) {
//     errs() << "this value found in the symboltable: ";
//     v->print(errs());
//     errs() << " (type ";
//     v->getType()->print(errs());
//     errs() << "\n is a constant expr? " << isa<llvm::Constant>(v) << "\n";
//     errs() << "\n is a GV? " << isa<llvm::GlobalValue>(v) << "\n";
//     errs() << "\n is a blockaddr? " << isa<llvm::BlockAddress>(v) << "\n";
//     errs() << "\n is a int? " << isa<llvm::ConstantInt>(v) << "\n";
// }

// /// EnumerateValueSymbolTable - Insert all of the values in the specified symbol
// /// table into the values table.
// void EnumerateValueSymbolTable(const ValueSymbolTable &VST) {
//   for (ValueSymbolTable::const_iterator VI = VST.begin(), VE = VST.end();
//        VI != VE; ++VI)
//     EnumerateValue(VI->getValue());
// }


static void CreateSetTag(Value * GV, Instruction* existingInst, llvm::LLVMContext& context) {
    std::vector<Type*>AsmFuncTy_args;
    AsmFuncTy_args.push_back(GV->getType());
    FunctionType* AsmFuncTy = FunctionType::get(
        /*Result=*/GV->getType(), //IntegerType::get(mod->getContext(), 32), //Type::getVoidTy(context),
        /*Params=*/AsmFuncTy_args,
        /*isVarArg=*/false);

    // we need to create a volatile store so that the full value of the GV will be computed before we 
    // create our settag insn.
    errs() << " creating blessed storage\n";
    AllocaInst* blessed_storage = new AllocaInst(GV->getType(), "blessed_use", existingInst);
    errs() << " creating blessed LOAD\n";
    LoadInst* blessed_load = new LoadInst(blessed_storage, "blessed load", /* volatile = */ false, existingInst);
    blessed_load->setAlignment(4);

    // n.b.; gcc uses %0 but clang uses $0 to refer to the operand
    // "2" is the tag we are setting on the thing
    std::string riscv_set = "settag $0, 2";
    std::string x86_set = "addi $0, $0, 1337"; //int $$0x1337";
    InlineAsm* myinlineasm = InlineAsm::get(AsmFuncTy, riscv_set, "=r,r,~{dirflag},~{fpsr},~{flags}",true);
    //std::vector<Value*> asm_params;
    //asm_params.push_back(blessed_load);
    errs() << " creating bless-ing call\n";
    CallInst* asm_call = CallInst::Create(myinlineasm, blessed_load, "", existingInst);
    //asm_call->setCallingConv(CallingConv::C);
    asm_call->setTailCall(false);
    AttributeSet asm_call_PAL;
    {
        SmallVector<AttributeSet, 4> Attrs;
        AttributeSet PAS;
        {
            AttrBuilder B;
            B.addAttribute(Attribute::NoUnwind);
            PAS = AttributeSet::get(context, ~0U, B);
        }
        Attrs.push_back(PAS);
        asm_call_PAL = AttributeSet::get(context, Attrs);
    }
    asm_call->setAttributes(asm_call_PAL);

    errs() << " creating blessed STORE\n";
    StoreInst* blessed_store = new StoreInst(GV, blessed_storage, /* volatile = */ false, existingInst);


    if (isa<ConstantExpr>(existingInst)) { errs() << "constant expr\n"; return;  }
    if (isa<Constant>(existingInst)) { errs() << "constant nonexp\n";  return; } 
    
    blessed_store->replaceUsesOfWith(GV, asm_call);
    errs() << "NOT replacing!!\n";
}

bool HelloIne::runOnModule(Module &M) { 

  errs() << "starting on module " << M << "\n";
  Module::FunctionListType& flist = M.getFunctionList();
   // this gives us the LLVM::Value for each function; iterate through uses of
   // these values and replace them with "blessed" uses

  for (Module::FunctionListType::iterator it=flist.begin(); it!=flist.end(); ++it) {
      errs() << "********************\nstarting on function: ";
       errs() << it->getName() << "\n*****************\n";
       IsCodePointer(it, M.getContext(), 0);
   }

  ValueMap<Function*, SmallVector<Value*, 16> >::iterator iter;
  for (iter = UserFunctionMap.begin(); iter != UserFunctionMap.end(); ++iter) {
      Function * theFunction = iter->first;
      if (theFunction == NULL) { continue; }

      errs() << "function: " << theFunction->getName() << "\n";
      
      for(int i = 0; i != (iter->second).size(); i++) {
          Value * GV = (iter->second[i]);
          errs() << "\t\t\t: " << GV->getType() <<  "::" << GV->getName() << "\n";

          // insert a use!
          CreateSetTag(GV, theFunction->begin()->getFirstInsertionPt(), M.getContext());
      }
  }
  

   //EnumerateValueSymbolTable(M.getValueSymbolTable());
    //SmallVector<GlobalVariable *, 16> GlobalsToChange;
    //
    //errs() << "********************************************************************************\n";
    /*
for (Module::iterator F = M.begin(), E = M.end(); F != E; ++F) {
        if (F->isDeclaration())
            continue;

        if (IsCodePointer(F, 0)) {
            // hi
        }
    }
    */

// from lib/transforms/instrumentation/AddressSanitizer.cpp
// AddressSanitizerModule::InstrumentGlobals

/*
  for (auto &G : M.globals()) {
      //if (IsCodePointer(&G, 0)) GlobalsToChange.push_back(&G);
      GlobalsToChange.push_back(&G);
      //if (ShouldProtectType(&G, 0)) GlobalsToChange.push_back(&G);
  }
*/

//  GlobalVariable *ModuleName = createPrivateGlobalForString(
//      M, M.getModuleIdentifier(), /*AllowMerging*/ false);  

/*
  size_t n = GlobalsToChange.size();
  for (size_t i = 0; i < n; i++) {
    GlobalVariable *G = GlobalsToChange[i];
*/
    
    // Create string holding the global name (use global name from metadata
    // if it's available, otherwise just write the name of global variable).
//    GlobalVariable *Name = createPrivateGlobalForString(
//        M, "__fp" + G->getName(), /*AllowMerging*/ true);

// 
/*
    GlobalVariable::GlobalVariable(Module & M,
                                   Type * Ty,
                                   bool isConstant,
                                   LinkageTypes Linkage,
                                   Constant * Initializer,
                                   const Twine & Name = "",
                                   GlobalVariable * InsertBefore = nullptr,
                                   ThreadLocalMode TLMode = NotThreadLocal,
                                   unsigned AddressSpace = 0,
                                   bool isExternallyInitialized = false 
        )
*/
/*
    PointerType *PtrTy = cast<PointerType>(G->getType());
    Type *Ty = PtrTy->getElementType();

    GlobalVariable *NewGlobal = //new GlobalVariable(G);
        new GlobalVariable(M, Ty, G->isConstant(), G->getLinkage(), G->getInitializer(),
                           "__fp_" + G->getName(), G, G->getThreadLocalMode());
    errs() << "new global created " << NewGlobal << "\n";
    NewGlobal->copyAttributesFrom(G);
    G->replaceAllUsesWith(NewGlobal);
    errs() << "replaced " << G->getName() << "\n";
  }



    errs() << "********************************************************************************\n";
  // Loop over global variables.  We inform the solver about any internal global
  // variables that do not have their 'addresses taken'.  If they don't have
  // their addresses taken, we can propagate constants through them.
  for (Module::global_iterator G = M.global_begin(), E = M.global_end();
       G != E; ++G) {
      IsCodePointer(G, 0);
//    if (!G->isConstant() && G->hasLocalLinkage() && !IsCodePointer(G))
//      Solver.TrackValueOfGlobalVariable(G);
  }
*/

  // did we modify the module yes or no
  return true;
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


// for running with opt
char HelloIne::ID = 0;
static RegisterPass<HelloIne> X("HelloIne", "HelloIne World Pass", false, false);

// for running with clang
static void registerHelloIne(const PassManagerBuilder &, PassManagerBase &PM) {
    PM.add(new HelloIne());
}

static RegisterStandardPasses RegisterHelloIne(PassManagerBuilder::EP_EnabledOnOptLevel0, registerHelloIne);

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

