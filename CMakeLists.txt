cmake_minimum_required(VERSION 3.25)

project(sha256-comparison)

find_package(benchmark REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

add_library(HwLocIf INTERFACE)

if(WIN32)
    add_library(HwLoc STATIC IMPORTED)
    set_target_properties(HwLoc PROPERTIES
        IMPORTED_LOCATION "${VCPKG_INSTALLED_DIR}/${VCPKG_HOST_TRIPLET}/lib/hwloc.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${VCPKG_INSTALLED_DIR}/${VCPKG_HOST_TRIPLET}/include"
    )
    target_link_libraries(HwLocIf INTERFACE HwLoc)
else(WIN32)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(HWLOC REQUIRED IMPORTED_TARGET hwloc)
    target_link_libraries(HwLocIf INTERFACE PkgConfig::HWLOC)
endif(WIN32)


set(CMAKE_CXX_STANDARD 20)

add_compile_options(-mssse3 -msse4.1 -msse4.2 -mavx -mavx2 -msha -Wall -Wextra -pedantic -Wno-deprecated-declarations)

add_library(all_algorithms INTERFACE)

add_subdirectory(bitcoin)
target_link_libraries(all_algorithms INTERFACE bitcoin)
add_subdirectory(zedwood)
target_link_libraries(all_algorithms INTERFACE zedwood)

target_link_libraries(all_algorithms INTERFACE OpenSSL::SSL OpenSSL::Crypto)

set(main_src 
    "main.cpp"
)
add_executable(main ${main_src})
target_link_libraries(main all_algorithms)
target_link_libraries(main benchmark::benchmark benchmark::benchmark_main)
target_link_libraries(main HwLocIf)

set(test_src 
    "test.cpp"
)
add_executable(test ${test_src})
target_link_libraries(test all_algorithms)
target_link_libraries(test Catch2::Catch2WithMain)
