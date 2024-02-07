set(bitcoin_src 
    "sha256.cpp"
    "sha256_arm_shani.cpp"
    "sha256_avx2.cpp"
    "sha256_sse4.cpp"
    "sha256_sse41.cpp"
    "sha256_x86_shani.cpp"
)
set(bitcoin_headers
    "sha256.h"
)
add_library(bitcoin STATIC ${bitcoin_src} ${bitcoin_headers})
target_compile_definitions(bitcoin PRIVATE -DENABLE_X86_SHANI -DENABLE_SSE41 -DENABLE_AVX2 -DUSE_ASM)