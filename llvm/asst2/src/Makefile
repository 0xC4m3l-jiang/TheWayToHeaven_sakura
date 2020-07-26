.PHONY : clean run_li run_ae build
# 替换成你的so存放的路径
MODULE_PATH = /Users/0xc3m4l/CLionProjects/llvmlearn/Assignment2/build/src/
# 替换成你的so名
MODULE_NAME = libAssignment2.so
# 替换成你的pass名
OPTION_AE= -avail_expr
OPTION_LA= -liveness

CC = clang
CFLAGS = -O0 -Xclang -disable-O0-optnone -emit-llvm -S


run_li: build make_so
	opt -mem2reg -S -load ${MODULE_PATH}${MODULE_NAME} ${OPTION_LA}  nopt_liveness.ll -o opt_liveness.ll

run_ae : build make_so
	opt -mem2reg -S -load ${MODULE_PATH}${MODULE_NAME} ${OPTION_AE}  nopt_avail_expr.ll -o opt_avail_expr.ll


build : ../tests/avail_expr.c
	${CC} ${CFLAGS} ../tests/avail_expr.c -o nopt_avail_expr.ll
	${CC} ${CFLAGS} ../tests/liveness.c -o nopt_liveness.ll


make_so : avail_expr.cpp
	cd ../build; make; pwd



clean :
	rm -rf *.ll
