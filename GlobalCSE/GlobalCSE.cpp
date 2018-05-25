#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Dominators.h"
#include "AvailableExpressions.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>
#include <algorithm>

using namespace llvm;
using namespace std;

namespace {
  	struct GlobalCSE : public FunctionPass {
		static char ID; // Pass identification, replacement for typeid

		GlobalCSE() : FunctionPass(ID) {}
		void getAnalysisUsage(AnalysisUsage &Info) const
		{
			Info.addRequired<DominatorTreeWrapperPass>();
			Info.setPreservesAll();
		}

		bool replaceInstructionByDominator(Instruction *instr, DominatorTree *dom_tree)
		{
			Expression *expr = ExpressionFactory::getExpression(instr);

			auto start = expr->begin();
			auto end = expr->end();
			auto iter_to_del = start;
			for(auto it = start; it != end; ++it)
			{
				if(*it == instr)
				{
					iter_to_del = it;
					break;
				}
			}
			for(auto it = start; it != end; ++it)
			{
				if( *it != instr && dom_tree->dominates(*it, instr) )
				{
					Value *dominator = *it;
					//errs() << "Dominator instruction ";
					//dominator->printAsOperand(errs(), false);
					//errs() << "\n";
					instr->replaceAllUsesWith(dominator);
					expr->removeInstruction(iter_to_del);
					return true;
				}
			}
			return false;
		}

		bool runOnFunction(Function &F) override
		{
			// Calculate available expressions
			AvailableExpressions av_exprs;
			av_exprs.analyze(F);
			//av_exprs.PrintResult(F);
			
			// Find Dominator relationships
			DominatorTree &dom_tree = getAnalysis<DominatorTreeWrapperPass>().getDomTree();

			// This will be used to collect dead instructions whose uses will be made NULL
			vector<Instruction*> instructions_to_delete;

			for(BasicBlock &BB : F)
			{
				for(Instruction &Instr : BB)
				{
					if( Instr.isBinaryOp() && av_exprs.isExpressionAvailable(&Instr) )
					{
						//errs() << "For Basic Block ";
						//BB.printAsOperand(errs(), false);
						//errs() << "\n";
						//errs() << "Replacing instruction ";
						//Instr.printAsOperand(errs(), false);
						//errs() << "\n";
						
						// If the instruction is available at the entry of the basic block,
						// then it shouldn't be calculated again.
						// Replace it with any one of its dominators.
						if( replaceInstructionByDominator(&Instr, &dom_tree) )
							instructions_to_delete.push_back(&Instr);
					}
				}
			}

			for(Instruction *Instr : instructions_to_delete)
			{
				Instr->eraseFromParent();
			}
			
			return false;
		}
  	};
}

char GlobalCSE::ID = 0;
static RegisterPass<GlobalCSE> X("gcse", "Global common sub-expression elimination", false, false);
