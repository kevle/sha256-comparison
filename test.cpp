#include "algorithm_wrappers.h"

#include <catch2/catch_template_test_macros.hpp>

#include <cstdint>

TEMPLATE_TEST_CASE("Well-known values", "[sha256_well_known]", sha256_zedwood, sha256_openssl, sha256_openssl_deprecated
#ifdef _WIN32
    , sha256_bcrypt
#endif
) {
  TestType sha_obj;
  sha_obj.add_bytes(nullptr, 0);
  auto digest = sha_obj.digest();
  REQUIRE(digest == std::array<unsigned char, 32>{
                        0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
                        0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
                        0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
                        0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55});
}
