#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "Expression.h"

Expression::Expression(Value* op1, unsigned oper, Value* op2, bool is_commutative)
	: op1_(op1), operator_(oper), op2_(op2), is_commutative_(is_commutative)
{
}

bool Expression::isSameAs(const Expression &that) const
{
	if(operator_ == that.getOperator())
	{
		if( (op1_ == that.getFirstOperand())
				&& (op2_ == that.getSecondOperand()) )
		{
			return true;
		}
		else if(is_commutative_)
		{
			if( (op1_ == that.getSecondOperand())
					&& (op2_ == that.getFirstOperand()) )
			{
				return true;
			}
		}
	}
	return false;
}

string Expression::getKey() const
{
	string op1_str = op1_->getName();
	string operator_str(Instruction::getOpcodeName(operator_));
	string op2_str = op2_->getName();
	return op1_str+operator_str+op2_str;
}

void Expression::addInstruction(Instruction *Instr)
{
	using_instructions_.push_back(Instr);
}

void Expression::removeInstruction(vector<Instruction*>::iterator iter)
{
	using_instructions_.erase(iter);
}

void Expression::print() const
{
	op1_->printAsOperand(errs(), false);
	errs() << " " << Instruction::getOpcodeName(operator_) << " ";
	op2_->printAsOperand(errs(), false);
}

StringMap<Expression*> ExpressionFactory::expression_db;

Expression * ExpressionFactory::addExpression(Instruction *Instr)
{
	Expression *new_expr = new Expression(Instr->getOperand(0), Instr->getOpcode(), Instr->getOperand(1), Instr->isCommutative());
	string key = new_expr->getKey();

	auto elem_iter = expression_db.find(key);
	if( elem_iter != expression_db.end() )
	{
		Expression *existing_expr = (*elem_iter).second;
		existing_expr->addInstruction(Instr);
		return existing_expr;
	}
	else
	{
		new_expr->addInstruction(Instr);
		expression_db[key] = new_expr;
		return new_expr;
	}
}

Expression * ExpressionFactory::constructExpression(Instruction *Instr)
{
	Expression *new_expr = new Expression(Instr->getOperand(0), Instr->getOpcode(), Instr->getOperand(1), Instr->isCommutative());
	new_expr->addInstruction(Instr);
	return new_expr;
}

Expression * ExpressionFactory::getExpression(Instruction *Instr)
{
	Expression *new_expr = new Expression(Instr->getOperand(0), Instr->getOpcode(), Instr->getOperand(1), Instr->isCommutative());
	string key = new_expr->getKey();
	auto elem_iter = expression_db.find(key);
	return (*elem_iter).second;
}
