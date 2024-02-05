#include <benchmark/benchmark.h>

#include <array>
#include <cassert>
#include <cstring>
#include <limits>
#include <random>
#include <vector>

#include "zedwood/sha256.h"

// Round up
static std::uint64_t div_up(std::uint64_t a, std::uint64_t b) {
  if (b == 0)
    return 0;

  if (a % b == 0)
    return a / b;
  else
    return a / b + 1;
}

struct sha256_dummy {
  void add_bytes(unsigned char *bytes, std::size_t num) {}
  std::array<unsigned char, 32> digest() { return {}; }
};

struct zedwood {
  SHA256 ctx;
  zedwood() { ctx.init(); }
  inline void add_bytes(unsigned char *bytes, std::size_t num) {
    constexpr std::size_t max_bytes =
        (std::numeric_limits<unsigned int>::max)();
    unsigned int size = 0;
    for (std::size_t i = 0; i < num; i += size, bytes += size) {
      size = static_cast<unsigned int>((std::min)(max_bytes, num - i));
      ctx.update(bytes, size);
    }
  }
  std::array<unsigned char, 32> digest() {
    std::array<unsigned char, 32> tmp;
    ctx.final(tmp.data());
    return tmp;
  }
};

template <typename sha256_wrapper>
class data_fixture : public benchmark::Fixture {
public:
  std::vector<unsigned char> data;

  using sha256_wrapper_t = sha256_wrapper;

  void SetUp(::benchmark::State &state) {
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
      ->Range(1LL << 8, 1LL << 30);

// BENCHMARK_SHA256(sha256_dummy); // Do nothing
BENCHMARK_SHA256(zedwood);

BENCHMARK_MAIN();