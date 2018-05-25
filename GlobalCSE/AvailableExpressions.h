#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "Expression.h"
#include <map>
#include <set>
#include <memory>

using namespace llvm;
using namespace std;
		
typedef const Expression* ExprT;
typedef set<ExprT> ExprSetT;
typedef map<BasicBlock*,ExprSetT> BBToExprSetMapT;

class AvailableExpressions {
	public:

		void analyze(Function &);
		void Init(Function &F);
		void PopulateGeneratedExprs(Function &F);
		bool PopulateInputExprSet(BasicBlock &BB);
		bool PopulateOutputExprSet(BasicBlock &BB);
		void PrintResult(Function &F);

		bool isExpressionAvailable(Instruction *Instr);

		ExprSetT & getInputAvExprs(BasicBlock &BB);
		ExprSetT & getOutputAvExprs(BasicBlock &BB);
		ExprSetT & getGeneratedExprs(BasicBlock &BB);

	private:
		BBToExprSetMapT input_av_exprs;
		BBToExprSetMapT output_av_exprs;
		BBToExprSetMapT generated_exprs;
};
