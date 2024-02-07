set(zedwood_src 
    "sha256.cpp"
)
set(zedwood_headers
    "sha256.h"
)
add_library(zedwood STATIC ${zedwood_src} ${zedwood_headers})
