cmake_minimum_required(VERSION 3.10)
project(TicTacToe C)

# 设置C标准
set(CMAKE_C_STANDARD 99)

# 添加可执行文件
add_executable(tic_tac_toe tic_tac_toe.c)

# 平台特定配置
if(WIN32)
    # Windows环境，添加Windows特定库
    target_link_libraries(tic_tac_toe PRIVATE winmm)
else()
    # Unix-like系统（包括Linux和macOS）添加pthread库
    find_package(Threads REQUIRED)
    target_link_libraries(tic_tac_toe PRIVATE Threads::Threads)
endif()
