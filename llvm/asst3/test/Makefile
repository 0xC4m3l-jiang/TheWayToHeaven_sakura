.PHONY: all,test,clean

MODULE_PATH=YOUR_PATH_TO_LICM
MODULE_NAME=libLICM.so
OPTION_LICM=-loop-invariant-code-motion

loop.ll:
	clang -O0 -Xclang -disable-O0-optnone -emit-llvm -c -S loop.c -o loop.ll
	opt -mem2reg -S loop.ll -o loop.ll

test_licm: loop.ll  
	opt -load=${MODULE_PATH}${MODULE_NAME} ${OPTION_LICM} loop.ll -S -o loop_output.ll

clean:
	@- rm *.ll
	@ echo "workspace cleaned."
