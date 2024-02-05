cmake_minimum_required(VERSION 3.25)

project(sha256-comparison)

find_package(benchmark REQUIRED)
find_package(OpenSSL REQUIRED)

set(src 
    "main.cpp"
    "zedwood/sha256.cpp"
)

set(headers
    "zedwood/sha256.h"
)

add_executable(main ${src} ${headers})
set_property(TARGET main PROPERTY CXX_STANDARD 17)
target_link_libraries(main benchmark::benchmark benchmark::benchmark_main)
target_link_libraries(main OpenSSL::SSL OpenSSL::Crypto)
