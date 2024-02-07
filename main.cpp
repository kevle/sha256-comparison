#include <benchmark/benchmark.h>

#include "algorithm_wrappers.h"

#include <array>
#include <cstring>
#include <random>
#include <type_traits>

template <typename sha256_wrapper>
class data_fixture : public benchmark::Fixture {
public:
  std::vector<unsigned char> data;

  using sha256_wrapper_t = sha256_wrapper;

  void SetUp(::benchmark::State &state) {
    if constexpr(std::is_same_v<sha256_wrapper, sha256_bitcoin>)
    {
      SHA256AutoDetect(sha256_implementation::USE_ALL);
    }
    // Fill data with reproducibly random bytes
    std::mt19937_64 gen;
    std::int64_t size = state.range(0);
    assert(size % sizeof(std::uint64_t) == 0);
    data.reserve(size);
    for (std::uint64_t i = 0; i < size; i += sizeof(std::uint64_t)) {
      auto rnd = gen();
      std::array<unsigned char, sizeof(std::uint64_t)> tmp;
      std::memcpy(tmp.data(), &rnd, sizeof(std::uint64_t));
      data.insert(data.end(), tmp.begin(), tmp.end());
    }
  }
  void TearDown(::benchmark::State &state) { data.clear(); }
};

#define BENCHMARK_SHA256(SHA256_TYPE)                                          \
  BENCHMARK_TEMPLATE_DEFINE_F(data_fixture, BM_##SHA256_TYPE, SHA256_TYPE)     \
  (::benchmark::State & state) {                                               \
    for (auto _ : state) {                                                     \
      SHA256_TYPE sha256_obj;                                                  \
      sha256_obj.add_bytes(data.data(), data.size());                          \
      auto result = sha256_obj.digest();                                       \
      benchmark::DoNotOptimize(result);                                        \
      benchmark::ClobberMemory();                                              \
    }                                                                          \
    state.SetBytesProcessed(int64_t(state.iterations()) *                      \
                            int64_t(state.range(0)));                          \
  }                                                                            \
  BENCHMARK_REGISTER_F(data_fixture, BM_##SHA256_TYPE)                         \
      ->Range(1LL << 8, 1LL << 16)                                             \
      ->Name(#SHA256_TYPE);

// BENCHMARK_SHA256(sha256_dummy); // Do nothing
BENCHMARK_SHA256(sha256_zedwood);
BENCHMARK_SHA256(sha256_bitcoin);
BENCHMARK_SHA256(sha256_openssl_deprecated);
BENCHMARK_SHA256(sha256_openssl);
#ifdef USE_NSS
BENCHMARK_SHA256(sha256_libnss);
#endif
#ifdef _WIN32
BENCHMARK_SHA256(sha256_bcrypt);
#endif

BENCHMARK_MAIN();