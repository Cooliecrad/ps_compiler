cmake_minimum_required(VERSION 3.13.0)

project(Ptil0psis's_Compiler)

# 包含uthash库

# 打开保存中间文件，方便查看宏扩展情况
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -save-temps=obj")

add_library(syna syntax_analysis.cpp lex_scanner.cpp lex_analysis.cpp)

# 词法分析器 测试用例
add_executable(lexa_test __lexa_test.cpp)
target_link_libraries(lexa_test syna)

# 词法扫描器 测试用例
add_executable(lexs_test __lexs_test.cpp)
target_link_libraries(lexs_test syna)

#  递归语法分析器 测试用例
add_executable(syna_test __syna_test.cpp)
target_link_libraries(syna_test syna)
