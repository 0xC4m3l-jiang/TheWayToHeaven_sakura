cmake_minimum_required(VERSION 3.16)
project(llvm_learn)
set(CMAKE_CXX_STANDARD 11)
# 设置 llvm 的path
set(LLVM_HOME /usr/local/opt/llvm)
set(LLVM_DIR ${LLVM_HOME}/lib/cmake/llvm)
# 查看 llvm 地址是否绑定成功
MESSAGE(STATUS "LLVM_HOME = ${LLVM_HOME}")
MESSAGE(STATUS "LLVM_DIR = ${LLVM_DIR}")
# 设置llvm 的库函数
# 找到 LLVM_DIR 路径下的 LLVM_CONFIG.cmake 文件
find_package(LLVM REQUIRED CONFIG)

# 编译时的参数
add_definitions(${LLVM_DEFINITIONS})
# llvm 的include 文件夹位置
include_directories(${LLVM_INCLUDE_DIRS})
# llvm lib文件夹
link_directories(${LLVM_LIBRARY_DIRS})

MESSAGE("_-------------------------------")
MESSAGE(${LLVM_DEFINITIONS})
MESSAGE(${LLVM_LIBRARY_DIRS})
MESSAGE(${LLVM_INCLUDE_DIRS})
message("---------------------------------")
## 编译的文件夹
#add_subdirectory(src)

if (${LLVM_VERSION_MAJOR} VERSION_GREATER_EQUAL 10)
    set(CMAKE_CXX_STANDARD 14)
endif ()
add_subdirectory(src)
