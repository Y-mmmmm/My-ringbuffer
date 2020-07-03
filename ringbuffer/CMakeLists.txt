#声明要求的cmake最低版本
cmake_minimum_required( VERSION 3.0)

#声明一个cmake工程
project(testRingBuffer)

#添加当前文件夹下所有".c"文件
aux_source_directory(. SRCS)

#添加一个可执行程序
#语法：add_executable(程序名 源代码文件)
add_executable(testRingBuffer ${SRCS})

