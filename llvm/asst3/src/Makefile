.PHONY : clean run build
# 替换成你的so存放的路径
MODULE_PATH = /Users/0xc3m4l/CLionProjects/llvmlearn/Assignment3/build/src/
# 替换成你的so名
MODULE_NAME = libAssignment3.so
# 替换成你的pass名
OPTION_LP= -loop-invariant-code-motion

CC = clang
CFLAGS = -O0 -Xclang -disable-O0-optnone -emit-llvm -S


run: build make_so
	opt -mem2reg -S -load ${MODULE_PATH}${MODULE_NAME} ${OPTION_LP}  nopt_loop.ll -o opt_loop.ll


build : ../test/loop.c
	${CC} ${CFLAGS} ../test/loop.c -o nopt_loop.ll


make_so : loop_invariant_code_motion.cpp
	cd ../build; make; pwd



clean :
	rm -rf *.ll
