#include "llvm/IR/Value.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/StringMap.h"
#include <string>
#include <vector>

using namespace llvm;
using namespace std;

class Expression
{
	Value* op1_;
	unsigned operator_;
	Value* op2_;
	bool is_commutative_;
	vector<Instruction*> using_instructions_;
	
	public:
	Expression(Value* op1, unsigned oper, Value* op2, bool is_commutative);

	Value* getFirstOperand() const { return op1_; }
	unsigned getOperator() const { return operator_; }
	Value* getSecondOperand() const { return op2_; }

	bool isSameAs(const Expression &that) const;
	void addInstruction(Instruction *Instr);
	void removeInstruction(vector<Instruction*>::iterator iter);
	string getKey() const;
	void print() const;

	vector<Instruction*>::iterator begin() { return using_instructions_.begin(); }
	vector<Instruction*>::iterator end() { return using_instructions_.end(); }
};

class ExpressionFactory
{
	static StringMap<Expression*> expression_db;

	public:
	static Expression * addExpression(Instruction *instr);
	static Expression * constructExpression(Instruction *instr);
	static Expression * getExpression(Instruction *instr);
};
