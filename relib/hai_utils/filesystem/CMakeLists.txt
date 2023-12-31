CMAKE_MINIMUM_REQUIRED(VERSION 3.7.2)
PROJECT(
        ghcfilesystem,
        VERSION 1.5.14
)

IF (POLICY CMP0077)
    CMAKE_POLICY(SET CMP0077 NEW)
ENDIF ()
IF (POLICY CMP0110)
    CMAKE_POLICY(SET CMP0110 NEW)
ENDIF ()
IF (NOT DEFINED CMAKE_CXX_STANDARD)
    SET(CMAKE_CXX_STANDARD 11)
    SET(CMAKE_CXX_STANDARD_REQUIRED ON)
    IF (NOT CYGWIN)
        SET(CMAKE_CXX_EXTENSIONS OFF)
    ENDIF ()
ENDIF ()
IF (CMAKE_CXX_STANDARD LESS 11)
    MESSAGE(FATAL_ERROR "CMAKE_CXX_STANDARD is less than 11, ghc::filesystem only works with C++11 and above.")
ENDIF ()
MESSAGE(STATUS "System name: ${CMAKE_SYSTEM_NAME}")
MESSAGE(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID}")
MESSAGE(STATUS "CMAKE_CXX_COMPILE_FEATURES: ${CMAKE_CXX_COMPILE_FEATURES}")
ADD_LIBRARY(ghc_filesystem INTERFACE)
ADD_LIBRARY(ghcFilesystem::ghc_filesystem ALIAS ghc_filesystem)
TARGET_INCLUDE_DIRECTORIES(ghc_filesystem INTERFACE
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                           $<INSTALL_INTERFACE:include>)
TARGET_COMPILE_OPTIONS(ghc_filesystem INTERFACE "$<$<C_COMPILER_ID:MSVC>:/utf-8>")
TARGET_COMPILE_OPTIONS(ghc_filesystem INTERFACE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")