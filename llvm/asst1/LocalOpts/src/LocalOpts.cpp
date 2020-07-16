//
// Created by 骆驼 on 2020/7/16.
//
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

#include <iostream>

using namespace std;
using namespace llvm;

namespace {
    struct LocalOpts : public FunctionPass {
        static char ID;
        LocalOpts() : FunctionPass(ID) {
            errs() << "llvm init \n";
        }
        // We don't modify the program, so we preserve all analysis.
        virtual void getAnalysisUsage(AnalysisUsage & AU) const override
        {
            AU.setPreservesAll();
        }
        ~LocalOpts() override{};


        bool runOnFunction(Function &F) override {
            std::list<Instruction*> deleteInst;
            for(BasicBlock &BB : F){
                for(Instruction &I : BB){
                    // 操作参数是否是 两个
                    if(I.getNumOperands() == 2) {
                        int64_t ConstVal1=0, ConstVal2=0;
                        // 得到第一 第二个 参数的地址。
                        bool opted = false;
                        Value *Opd1 = I.getOperand(0);
                        Value *Opd2 = I.getOperand(1);
                        // 得到参数的值
                        if (isa<ConstantInt>(Opd1)) {
                            ConstVal1 = dyn_cast<ConstantInt>(Opd1)->getSExtValue();
                        }
                        if (isa<ConstantInt>(Opd2)) {
                            ConstVal2 = dyn_cast<ConstantInt>(Opd2)->getSExtValue();
                        }
                        switch (I.getOpcode()) {
                            // x+0 = x ; 0 + x = x
                            case Instruction::Add:
                                errs() << "Add\n";
                                errs() << ConstVal1 << " " << ConstVal2 << "\n";
                                if(ConstVal1 == 0){
                                    I.replaceAllUsesWith(Opd2);
                                    opted  = true;
                                }else if(ConstVal2 == 0){
                                    I.replaceAllUsesWith(Opd1);
                                    opted = true;
                                }
                                if(opted){
                                    if(I.isSafeToRemove())
                                        deleteInst.push_back(&I);
                                }
                                errs() << "---------------------\n";
                                break;

                                // x-0 = x ; x-x=0
                            case Instruction::Sub:
                                errs() << "Sub\n";
                                errs() << ConstVal1 << " " << ConstVal2 << "\n";
                                if(ConstVal2 == 0){
                                    I.replaceAllUsesWith(Opd2);
                                    opted  = true;
                                }else if(ConstVal2 == ConstVal1)
                                {
                                    I.replaceAllUsesWith(ConstantInt::getSigned(I.getType(),0));
                                    opted = true;
                                }
                                if(opted){
                                    if(I.isSafeToRemove())
                                        deleteInst.push_back(&I);
                                }
                                errs() << "---------------------\n";
                                break;

                                // 2*4 => 8
                            case Instruction::Mul:
                                errs() << "Mul\n";
                                errs() << ConstVal1 << " " << ConstVal2 << "\n";
                                // x * 0 = 0 ; 0 * x= 0
                                if(ConstVal1==0 || ConstVal2==0){
                                    I.replaceAllUsesWith(ConstantInt::getSigned(I.getType(),0));
                                    opted = true;
                                }else{
                                    I.replaceAllUsesWith(ConstantInt::getSigned(I.getType(),ConstVal1*ConstVal2));
                                    opted = true;
                                }
                                if(opted){
                                    if(I.isSafeToRemove())
                                        deleteInst.push_back(&I);
                                }
                                errs() << "---------------------\n";
                                break;

                            case Instruction::SDiv:
                                break;

                            default:
                                break;
                        }
                    }
                }
            }
            for(Instruction* inst : deleteInst)
                inst->eraseFromParent();
            return true;
        }




    };
}

char LocalOpts::ID = 0;
static RegisterPass<LocalOpts> X("local-opts", "my LocalOpts",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);

