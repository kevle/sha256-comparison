cmake_minimum_required(VERSION 3.25)

project(sha256-comparison)

find_package(benchmark REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
# if(WIN32)
#     find_package(PkgConfig REQUIRED)
#     pkg_check_modules(NSS REQUIRED nss-softokn nss-util nss) #
# endif()

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-mssse3 -msse4.1 -msse4.2 -mavx -mavx2 -msha)

add_subdirectory(bitcoin)
add_subdirectory(zedwood)

add_library(all_algorithms INTERFACE)
target_link_libraries(all_algorithms INTERFACE OpenSSL::SSL OpenSSL::Crypto bitcoin zedwood)
# if(WIN32)
#     target_link_libraries(main ${NSS_LIBRARIES})
#     target_include_directories(main PUBLIC ${NSS_INCLUDE_DIRS})
#     target_compile_options(main PUBLIC ${NSS_CFLAGS_OTHER})
# endif()

set(main_src 
    "main.cpp"
)
add_executable(main ${main_src})
target_link_libraries(main all_algorithms)
target_link_libraries(main benchmark::benchmark benchmark::benchmark_main)

set(test_src 
    "test.cpp"
)
add_executable(test ${test_src})
target_link_libraries(test all_algorithms)
target_link_libraries(test Catch2::Catch2WithMain)
