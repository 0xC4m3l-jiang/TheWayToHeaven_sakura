.PHONY : all clean run_ol build_ll

SoPath = /Users/0xc3m4l/CLionProjects/llvmlearn/MyPass/build/src/
SoName = libFunctionInfo.so

OPTION_LO= -local-opts
OPTION_FI= -function-info

all : build_ll run_ol

run_ol : build_ll
	opt -load ${SoPath}${SoName} ${OPTION_FI} < FunctionInfo_loop.ll > /dev/null

build_ll :
	clang -emit-llvm -S loop.c -o FunctionInfo_loop.ll

clean :
	rm *.ll