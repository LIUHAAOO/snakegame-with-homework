cmake_minimum_required(VERSION 3.10)
project(snakegame-with-homework)

set(CMAKE_CXX_STANDARD 11)

# 包含头文件路径
include_directories(C:/Users/LIU/Desktop/EasyX/include)

# 添加库的路径
link_directories(C:/Users/LIU/Desktop/EasyX/lib64)

# 添加可执行文件
add_executable(snakegame-with-homework snake.cpp)
# 链接库文件
target_link_libraries(snakegame-with-homework easyx)
