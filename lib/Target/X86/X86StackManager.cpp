//
//  RMDStackManager.cpp
//  TestBackEnd
//
//  Created by Bryce Holton on 6/23/14.
//  Copyright (c) 2014 Bryce Holton. All rights reserved.
//

#include "X86.h"
#include "X86TargetMachine.h"
#include "X86InstrInfo.h"
#include "MCTargetDesc/X86BaseInfo.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DebugInfo.h"

#include <string.h>
#include <string>

using namespace llvm;

namespace llvm
{
	struct X86StackManager : public MachineFunctionPass
    {

    public:
        TargetMachine &TM;
        static char ID;// Pass identification, replacement for typeid
        
        //**********************************************************************
        // constructor
        //**********************************************************************
        X86StackManager(TargetMachine &tm) : MachineFunctionPass(ID), TM(tm) {}
        
        bool compareCallToFunctionNames(std::string function_name)
        {
            std::string function_names[] = {"__sstore", "__sload", "__g2l", "__l2g"};
            for (int i = 0; i < 5; i++)
            {
                if (function_names[i].compare(function_name) == 0)
                {
                    return true;
                }
            }
            return false;
        }
        bool runOnMachineFunction(MachineFunction &MF)
        {
//            errs() << MF.getName() <<"\n";
//            MachineFrameInfo *MFI = MF.getFrameInfo();
//            errs() << MFI->estimateStackSize(MF) << "\n";
            
            // Iterate over each basic block
            for(MachineFunction::iterator bItr = MF.begin(); bItr != MF.end(); ++bItr)
            {
                // Iterate over each instruction
                for(MachineBasicBlock::iterator iItr = bItr->begin(); iItr != bItr->end(); ++iItr)
                {
                    // Find out if instruction is a function call.
                    if ( iItr->isCall() )
                    {
                        MachineOperand MO = iItr->getOperand(0); // Get first operand of machine instruction, contains function name.
                        const GlobalValue *GV = MO.getGlobal(); // The first operand is actually a global address.
                        std::string funcName = GV->getName(); // The global address of a function call has a function name property.
                        
                        // Find out if it is not a call to any of our libraries.
                        if ( !compareCallToFunctionNames(funcName) )
                        {
                            //Get Target Info to generate instructions
                            const TargetSubtargetInfo &TM = MF.getSubtarget();
                            const TargetInstrInfo *TII = TM.getInstrInfo();
                            MachineBasicBlock::instr_iterator I = bItr->instr_begin();
                            DebugLoc DL3 = bItr->findDebugLoc(I);
                            
                            //Generate two additional function call instructions
//                            MachineInstrBuilder sstoreInst =  llvm::BuildMI(MF, DL3, TII->get(X86::CALL32r)).addExternalSymbol("__sstore");
//                            MachineInstrBuilder sloadInst =  llvm::BuildMI(MF, DL3, TII->get(X86::CALL32r)).addExternalSymbol("__sload");
                            
                            //Insert one instruction before and one instruction after the function call.
//                            bItr->insert(iItr, sstoreInst);
//                            bItr->insertAfter(iItr++, sloadInst);
                        }
                    }
                }
            }
            return true;
        }
    };
    
    char X86StackManager::ID = 0;
	FunctionPass *createStackManager(X86TargetMachine &TM)
	{
		return new X86StackManager(TM);
	}
}
