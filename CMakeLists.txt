cmake_minimum_required(VERSION 3.25)

project(sha256-comparison)

find_package(benchmark REQUIRED)
find_package(OpenSSL REQUIRED)
if(WIN32)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(NSS REQUIRED nss-softokn nss-util nss) #
endif()
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
if(WIN32)
    target_link_libraries(main ${NSS_LIBRARIES})
    target_include_directories(main PUBLIC ${NSS_INCLUDE_DIRS})
    target_compile_options(main PUBLIC ${NSS_CFLAGS_OTHER})
endif()
