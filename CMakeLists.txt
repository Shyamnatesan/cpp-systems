cmake_minimum_required(VERSION 3.24)

project(cpp-systems LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" ON)
if(ENABLE_SANITIZERS)
  add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address,undefined)
endif()


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
add_experiment(lock_free_ring_buffer)
add_experiment(arena)
