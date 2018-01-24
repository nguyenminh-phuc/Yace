cmake_minimum_required(VERSION 3.9.2 FATAL_ERROR)
cmake_policy(VERSION 3.9.2)

project(Yace)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (CMAKE_VERSION VERSION_LESS "3.10" AND MSVC)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_LEGACY_CYGWIN_WIN32 0)
set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)
set(INSTALL_DIR "${CMAKE_BINARY_DIR}/bin")

configure_file(
   "${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
   "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
   IMMEDIATE @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake_uninstall.cmake)

if (WIN32)
   include(CheckTypeSize)
   check_type_size(void* SIZEOF_VOID_PTR)
   if (${SIZEOF_VOID_PTR} EQUAL 4)
      set(ARCHITECTURE "x86")
   elseif(${SIZEOF_VOID_PTR} EQUAL 8)
      set(ARCHITECTURE "x64")
   endif()
   set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
   set(COMPILER_WARNINGS "/W4 /WX")
elseif (UNIX)
   set(CMAKE_INSTALL_RPATH ${INSTALL_DIR})
   set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
   set(COMPILER_WARNINGS "-Wall -Wextra -Werror -pedantic")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_WARNINGS}")

add_subdirectory("src/Yace")
add_subdirectory("examples")

if (DEFINED CMAKE_BUILD_TYPE)
   message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()
message("BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")
if (DEFINED ARCHITECTURE)
   message("ARCHITECTURE: ${ARCHITECTURE}")
endif()
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message("INSTALL_DIR: ${INSTALL_DIR}")