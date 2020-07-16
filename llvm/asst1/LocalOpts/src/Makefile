.PHONY : all clean run_ol build_ll make_so

SoPath = /Users/0xc3m4l/CLionProjects/llvmlearn/LocalOpts/build/src/
SoName = libLocalOpts.so

OPTION_LO= -local-opts

all : build_ll run_ol make_so

run_ol : build_ll make_so
	opt -mem2reg -S -load ${SoPath}${SoName} ${OPTION_LO}  loop.ll -o m2r_loop.ll

make_so :
	cd ../build; make; pwd

build_ll :
	clang -O0 -Xclang -disable-O0-optnone -emit-llvm -S loop.c -o loop.ll

clean :
	rm *.ll
