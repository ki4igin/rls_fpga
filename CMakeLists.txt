cmake_minimum_required(VERSION 3.16)

project(rls_fpga LANGUAGES CXX)

add_library(rls_fpga
    axi_dsp.cpp
    dma_channel.cpp
    fpga_dma.cpp
    misc.cpp
    rls.cpp
)

target_include_directories(rls_fpga
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(rls_fpga PUBLIC cxx_std_17)
