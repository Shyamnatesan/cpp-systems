cmake_minimum_required(VERSION 3.24)

project(cpp-systems LANGUAGES CXX)

# simpledb/ is a separate tree and is intentionally not part of this build.

if(PROJECT_IS_TOP_LEVEL AND CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR
    "In-source builds are not supported. Configure with: cmake -S . -B build")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_SANITIZERS
  "Enable AddressSanitizer and UndefinedBehaviorSanitizer" ON)

add_library(shyam INTERFACE)
add_library(shyam::shyam ALIAS shyam)
target_include_directories(shyam INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_compile_features(shyam INTERFACE cxx_std_23)

add_library(shyam_warnings INTERFACE)
add_library(shyam::warnings ALIAS shyam_warnings)
target_compile_options(shyam_warnings INTERFACE
  -Wall -Wextra -Wpedantic -Werror)

add_library(shyam_sanitizers INTERFACE)
add_library(shyam::sanitizers ALIAS shyam_sanitizers)
if(ENABLE_SANITIZERS)
  target_compile_options(shyam_sanitizers INTERFACE
    -fsanitize=address,undefined -fno-omit-frame-pointer)
  target_link_options(shyam_sanitizers INTERFACE
    -fsanitize=address,undefined)
endif()

find_package(Threads REQUIRED)

function(shyam_add_example name)
  set(target "shyam_${name}")
  add_executable(${target} examples/${name}/main.cpp)
  target_link_libraries(${target} PRIVATE
    shyam::shyam
    shyam::warnings
    shyam::sanitizers)
  set_target_properties(${target} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
endfunction()

shyam_add_example(vector)
shyam_add_example(malloc_stack)
shyam_add_example(simple_alloc)
shyam_add_example(mem_pool)
shyam_add_example(unique_ptr)
shyam_add_example(arena)
shyam_add_example(mutex)
shyam_add_example(lock_free_ring_buffer)
shyam_add_example(l1_cold_read_latency)
shyam_add_example(template_metaprogramming)
shyam_add_example(all_headers)

target_link_libraries(shyam_mutex PRIVATE Threads::Threads)
