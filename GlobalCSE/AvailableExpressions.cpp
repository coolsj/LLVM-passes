#include "llvm/IR/CFG.h"
#include "AvailableExpressions.h"

void AvailableExpressions::Init(Function &F)
{
	input_av_exprs.clear();
	output_av_exprs.clear();
	ExprSetT empty_set;
	for(auto &BB : F)
	{
		input_av_exprs[&BB] = empty_set;
		output_av_exprs[&BB] = empty_set;
	}
}

void AvailableExpressions::PopulateGeneratedExprs(Function &F)
{
	for(auto &BB : F)
	{
		ExprSetT generated_expr_set;
		for(auto &Instr : BB)
		{
			if(Instr.isBinaryOp())
			{
				ExprT new_expr = ExpressionFactory::addExpression(&Instr);
				generated_expr_set.insert(new_expr);
			}
		}
		generated_exprs[&BB] = generated_expr_set;
	}
}

bool AvailableExpressions::PopulateInputExprSet(BasicBlock &BB)
{
	size_t input_set_orig_size = input_av_exprs[&BB].size();
	ExprSetT new_input_set;
	unsigned pred_count = 0;
	for (auto it = pred_begin(&BB); it != pred_end(&BB); ++it)
	{
		BasicBlock *Pred = *it;
		pred_count++;
		ExprSetT pred_expr_set = output_av_exprs[Pred];
		if(pred_count == 1)
		{
			new_input_set = pred_expr_set;
		}
		else
		{
			ExprSetT temp_expr_set;
			for(auto curr_set_expr : new_input_set)
			{
				for(auto pred_set_expr : pred_expr_set)
				{
					if( curr_set_expr->isSameAs(*pred_set_expr) )
						temp_expr_set.insert(curr_set_expr);
				}
			}
			new_input_set = temp_expr_set;
		}
	}
	size_t input_set_new_size = new_input_set.size();
	input_av_exprs[&BB] = new_input_set;
	return (input_set_orig_size == input_set_new_size);
}

bool AvailableExpressions::PopulateOutputExprSet(BasicBlock &BB)
{
	size_t output_set_orig_size = output_av_exprs[&BB].size();
	ExprSetT new_output_set;
	new_output_set = generated_exprs[&BB];
	for(auto input_set_expr : input_av_exprs[&BB])
	{
		new_output_set.insert(input_set_expr);
	}
	size_t output_set_new_size = new_output_set.size();
	output_av_exprs[&BB] = new_output_set;
	return (output_set_orig_size == output_set_new_size);
}

bool AvailableExpressions::isExpressionAvailable(Instruction *Instr)
{
	Expression * new_expr = ExpressionFactory::constructExpression(Instr);
	BasicBlock *parent_BB = Instr->getParent();
	auto iter = input_av_exprs.find(parent_BB);
	if( iter != input_av_exprs.end() )
	{
		ExprSetT av_exprs = (*iter).second;
		for(Expression const *expr : av_exprs)
		{
			if( expr->isSameAs(*new_expr) )
			{
				return true;
			}
		}
	}
	return false;
}

ExprSetT & AvailableExpressions::getInputAvExprs(BasicBlock &BB)
{
	return input_av_exprs[&BB];
}

ExprSetT & AvailableExpressions::getOutputAvExprs(BasicBlock &BB)
{
	return output_av_exprs[&BB];
}

ExprSetT & AvailableExpressions::getGeneratedExprs(BasicBlock &BB)
{
	return generated_exprs[&BB];
}

void AvailableExpressions::PrintResult(Function &F)
{
	for(auto &BB : F)
	{
		errs() << "Basic Block ";
		BB.printAsOperand(errs(), false);
		errs() << "\n";

		errs() << "  Input set: \{ ";
		unsigned count = 0;
		for(auto expr : input_av_exprs[&BB])
		{
			count++;
			if( count != 1 )
			{
				errs() << ", ";
			}
			expr->print();
		}
		errs() << "\}\n";

		errs() << "  Output set: \{ ";
		count = 0;
		for(auto expr : output_av_exprs[&BB])
		{
			count++;
			if( count != 1 )
			{
				errs() << ", ";
			}
			expr->print();
		}
		errs() << "\}\n";
	}
}

void AvailableExpressions::analyze(Function &F)
{
	Init(F);
	PopulateGeneratedExprs(F);

	bool is_fixed_point_reached = false;
	while(!is_fixed_point_reached)
	{
		for(auto &BB : F)
		{
			is_fixed_point_reached = PopulateInputExprSet(BB);
			is_fixed_point_reached &= PopulateOutputExprSet(BB);
		}
	}
}
