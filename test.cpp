#include "algorithm_wrappers.h"

#include <catch2/catch_template_test_macros.hpp>

#include <cstdint>
#include <cstring>

#ifdef _WIN32
#define SHA256_BCRYPT , sha256_bcrypt
#else
#define SHA256_BCRYPT
#endif

#ifdef BITCOIN_IMPL
#define SHA256_BITCOIN , sha256_bitcoin
#else
#define SHA256_BITCOIN
#endif

TEMPLATE_TEST_CASE("Well-known values", "[sha256_well_known]", sha256_zedwood,
                   sha256_openssl,
                   sha256_openssl_deprecated SHA256_BCRYPT SHA256_BITCOIN) {
  {
    TestType sha_obj;
    sha_obj.add_bytes(nullptr, 0);
    auto digest = sha_obj.digest();
    REQUIRE(digest == std::array<unsigned char, 32>{
                          0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14,
                          0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
                          0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
                          0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55});
  }
  {
    TestType sha_obj;
    const char *str = "gjdfjajbsdtejewtjwtersfdfsdfsdfsdghthertertqwerwer";
    sha_obj.add_bytes(reinterpret_cast<const unsigned char *>(str),
                      std::strlen(str));
    auto digest = sha_obj.digest();
    REQUIRE(digest == std::array<unsigned char, 32>{
                          0xf7, 0x55, 0x9d, 0x5a, 0x69, 0xb0, 0xd6, 0xd2,
                          0xb9, 0x1b, 0xfc, 0x24, 0x47, 0x67, 0x50, 0x98,
                          0x72, 0x15, 0x7b, 0x4d, 0xd3, 0x81, 0x7a, 0xce,
                          0x04, 0xfb, 0x11, 0xb6, 0x76, 0x7e, 0x84, 0x93});
  }
}
