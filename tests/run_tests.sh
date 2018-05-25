#!/bin/bash
for test in `cat tests.txt`
do
	echo "Running $test.."
	testname=${test/\.c/""}
	mkdir $testname
	pushd $testname > /dev/null
    
	bcfile=${testname}".bc"
	echo "Generating llvm IR using clang at $bcfile.."
	clang -emit-llvm -c ../$test -o $bcfile
	llvm-dis $bcfile

	regbcfile="reg_"${testname}".bc"
	echo "Performing mem2reg on $bcfile to generate $regbcfile.."
	opt -mem2reg $bcfile -o $regbcfile
	llvm-dis $regbcfile
	
	lcsebcfile="lcse_"${testname}".bc"
	echo "Performing Local CSE on $regbcfile to generate $lcsebcfile.."
	opt -load `llvm-config --libdir`/LLVMLocalCSE.so -lcse $regbcfile -o $lcsebcfile
	llvm-dis $lcsebcfile
	
	gcsebcfile="gcse_"${testname}".bc"
	echo "Performing Global CSE on $lcsebcfile to generate $gcsebcfile.."
	opt -load `llvm-config --libdir`/LLVMGlobalCSE.so -gcse $lcsebcfile -o $gcsebcfile
	llvm-dis $gcsebcfile
	
	popd > /dev/null
    echo ""
done
