#include "llvm/IR/BasicBlock.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <vector>
#include <map>
#include <tuple>
using namespace llvm;
using namespace std;

namespace {
  	struct LocalCSE : public BasicBlockPass {
		static char ID; // Pass identification, replacement for typeid

		// data structure to track expressions
		class expression
		{
			Value* op1_;
			unsigned operator_;
			Value* op2_;
			vector<Instruction*> all_instrs;
			
			public:
			expression(Value* op1, unsigned oper, Value* op2)
				: op1_(op1), operator_(oper), op2_(op2)
			{
			}

			Value* getFirstOperand() { return op1_; }
			unsigned getOperator() { return operator_; }
			Value* getSecondOperand() { return op2_; }

			void addInstruction(Instruction* instr)
			{
				all_instrs.push_back(instr);
			}

			size_t size() { return all_instrs.size(); }
			vector<Instruction*>::iterator begin() { return all_instrs.begin(); }
			vector<Instruction*>::iterator end() { return all_instrs.end(); }
		};
		class expression_set
		{
			vector<expression> exprs;

			public:
			void addExpression(Instruction *instr)
			{
				for(auto &expr : exprs)
				{
					if( expr.getOperator() == instr->getOpcode() )
					{
						if( (expr.getFirstOperand() == instr->getOperand(0))
								&& (expr.getSecondOperand() == instr->getOperand(1)) )
						{
							expr.addInstruction(instr);
							return;
						}
						else if( instr->isCommutative() )
						{
							if( (expr.getSecondOperand() == instr->getOperand(0))
									&& (expr.getFirstOperand() == instr->getOperand(1)) )
							{
								expr.addInstruction(instr);
								return;
							}
						}
					}
				}
				expression new_expr( instr->getOperand(0), instr->getOpcode(), instr->getOperand(1) );
				new_expr.addInstruction(instr);
				exprs.push_back(new_expr);
			}

			bool doCse()
			{
				bool return_val = false;
				for(auto &expr : exprs)
				{
					if( expr.size() > 1 )
					{
						auto start = expr.begin();
						auto end = expr.end();
						Value *first_instr = nullptr;
						for(auto it = start; it != end; ++it)
						{
							if(it == start)
								first_instr = *it;
							else
							{
								Instruction *inst_to_replace = *it;
								BasicBlock::iterator ii(inst_to_replace);
								ReplaceInstWithValue(inst_to_replace->getParent()->getInstList(), ii, first_instr);
							}
						}
						return_val = true;
					}
				}
				return return_val;
			}
		};

		LocalCSE() : BasicBlockPass(ID) {}
		void getAnalysisUsage(AnalysisUsage &Info) const
		{
			Info.setPreservesAll();
		}

		bool runOnBasicBlock(BasicBlock &BB) override
		{
			expression_set all_exprs;
			for(auto & Instr : BB)
			{
				if( Instr.isBinaryOp() )
				{
					all_exprs.addExpression(&Instr);
				}
			}
			return all_exprs.doCse();
		}
  	};
}

char LocalCSE::ID = 0;
static RegisterPass<LocalCSE> X("lcse", "Local common sub-expression elimination", false, false);
