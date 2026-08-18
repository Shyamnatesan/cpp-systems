cmake_minimum_required(VERSION 3.24)

project(cpp-systems LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)


function(add_experiment name)
  add_executable(${name} src/${name}/main.cpp)
  target_include_directories(${name} PRIVATE src/${name})
endfunction()


add_experiment(vector)
add_experiment(malloc_stack)
add_experiment(simple_alloc)
add_experiment(mem_pool)
add_experiment(unique_ptr)
add_experiment(L1_cold_read_latency)
add_experiment(mutex)
