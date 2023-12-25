# Copyright 2020-2022 NVIDIA Corporation.  All rights reserved.
#
# Please refer to the NVIDIA end user license agreement (EULA) associated
# with this source code for terms and conditions that govern your use of
# this software. Any use, reproduction, disclosure, or distribution of
# this software and related documentation outside the terms of the EULA
# is strictly prohibited.

# 3.7 is required for FindVulkan module support in CMake.
cmake_minimum_required(VERSION 3.7)
project(hvidrec)

find_package(CUDA)
set(CUDA_HOST_COMPILER ${CMAKE_CXX_COMPILER})
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS};-gencode arch=compute_50,code=\"sm_50,compute_50\")
# CUDA 11.7 and later is compiled with /MT option (staticaly linked with C runtime), 
# since we are statically linking cudart_static.lib with the app, 
# we need to make sure that all .obj files linked are compiled with /MT to avoid linker warning LNK4098
if (WIN32 AND CUDA_VERSION_MAJOR GREATER_EQUAL 11 AND CUDA_VERSION_MINOR GREATER_EQUAL 7)
    set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS};-Xcompiler /MT)
endif ()
if (CMAKE_COMPILER_IS_GNUCC)
    if (NOT "${CUDA_NVCC_FLAGS}" MATCHES "-std=c\\+\\+11")
        list(APPEND CUDA_NVCC_FLAGS -std=c++11)
    endif ()
endif ()
aux_source_directory(. SRC)
cuda_add_library(${PROJECT_NAME} SHARED ${SRC})
if (WIN32 AND CUDA_VERSION_MAJOR GREATER_EQUAL 11 AND CUDA_VERSION_MINOR GREATER_EQUAL 7)
    target_compile_options(${PROJECT_NAME} PRIVATE "/MT")
endif ()
set_target_properties(${PROJECT_NAME} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
target_include_directories(${PROJECT_NAME} PUBLIC inc ${NVCodecSDK_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${CUDA_CUDA_LIBRARY} ${NVCodecSDK_LIBRARIES} hai_utils)
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PRIVATE HAI_EXPORT)
if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif ()
######################################################################################
add_executable(TEST_${PROJECT_NAME} test/AppEncCuda.cpp)
target_link_libraries(TEST_${PROJECT_NAME} ${PROJECT_NAME})
