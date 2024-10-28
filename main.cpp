#include <benchmark/benchmark.h>

#include "algorithm_wrappers.h"

#include <array>
#include <cstring>
#include <random>
#include <type_traits>
#include <iostream>

#include <hwloc.h>

static int getPhysicalCores()
{
  static const int num_physical_cores = []()
  {
    int num_physical_cores = 8;
    int res = 0;
    hwloc_topology_t topology;
    if (hwloc_topology_init(&topology) != 0)
    {
      goto cleanup;
    }
    if (hwloc_topology_load(topology) != 0)
    {
      goto cleanup;
    }
    res = hwloc_get_nbobjs_by_type(topology, hwloc_obj_type_t::HWLOC_OBJ_CORE);
    if (res > 0)
    {
      num_physical_cores = res;
    }
  cleanup:
    hwloc_topology_destroy(topology);
    if (res <= 0)
    {
      std::cerr << "Could not determine number of physical cores\n";
    }
    return num_physical_cores;
  }();

  return num_physical_cores;
}

template <typename sha256_wrapper>
class data_fixture : public benchmark::Fixture
{
public:
  static thread_local std::vector<unsigned char> data;

  using sha256_wrapper_t = sha256_wrapper;

  void SetUp(::benchmark::State &state)
  {
    if (state.thread_index() == 0)
    {
      global_md.reset(EVP_MD_fetch(NULL, "SHA256", NULL));
#ifdef BITCOIN_IMPL
      if constexpr (std::is_same_v<sha256_wrapper, sha256_bitcoin>)
      {
        SHA256AutoDetect(sha256_implementation::USE_ALL);
      }
#endif // BITCOIN_IMPL
    }
    // Fill data with reproducibly random bytes
    std::mt19937_64 gen;
    std::uint64_t size = static_cast<std::uint64_t>(state.range(0));
    assert(size % sizeof(std::uint64_t) == 0);
    data.reserve(size);
    for (std::uint64_t i = 0; i < size; i += sizeof(std::uint64_t))
    {
      auto rnd = gen();
      std::array<unsigned char, sizeof(std::uint64_t)> tmp;
      std::memcpy(tmp.data(), &rnd, sizeof(std::uint64_t));
      data.insert(data.end(), tmp.begin(), tmp.end());
    }
  }
  void TearDown(::benchmark::State &state)
  {
    if (state.thread_index() != 0)
      return;
    data.clear();
  }
};

template <typename sha256_wrapper>
thread_local std::vector<unsigned char> data_fixture<sha256_wrapper>::data;

#define BENCHMARK_SHA256(SHA256_TYPE)                                                      \
  BENCHMARK_TEMPLATE_DEFINE_F(data_fixture, BM_##SHA256_TYPE, SHA256_TYPE)                 \
  (::benchmark::State & state)                                                             \
  {                                                                                        \
    for (auto _ : state)                                                                   \
    {                                                                                      \
      SHA256_TYPE sha256_obj;                                                              \
      sha256_obj.add_bytes(data.data(), data.size());                                      \
      auto result = sha256_obj.digest();                                                   \
      benchmark::DoNotOptimize(result);                                                    \
      benchmark::ClobberMemory();                                                          \
    }                                                                                      \
    /* TODO: Check why multiplying with state.threads() became necessary. */               \
    /* https://github.com/google/benchmark/blob/main/docs/user_guide.md#custom-counters */ \
    state.SetBytesProcessed(int64_t(state.iterations()) *                                  \
                            int64_t(state.range(0)) * state.threads());                    \
  }                                                                                        \
  BENCHMARK_REGISTER_F(data_fixture, BM_##SHA256_TYPE)                                     \
      ->Range(1LL << 8, 1LL << 16)                                                         \
      ->ThreadRange(1, getPhysicalCores())                                                 \
      ->UseRealTime()                                                                      \
      ->Name(#SHA256_TYPE);

// BENCHMARK_SHA256(sha256_dummy); // Do nothing
BENCHMARK_SHA256(sha256_zedwood);
#ifdef BITCOIN_IMPL
BENCHMARK_SHA256(sha256_bitcoin);
#endif // BITCOIN_IMPL
BENCHMARK_SHA256(sha256_openssl_deprecated);
BENCHMARK_SHA256(sha256_openssl_oneshot);
BENCHMARK_SHA256(sha256_openssl_global);
BENCHMARK_SHA256(sha256_openssl);
#ifdef USE_NSS
BENCHMARK_SHA256(sha256_libnss);
#endif
#ifdef _WIN32
BENCHMARK_SHA256(sha256_bcrypt);
#endif

BENCHMARK_MAIN();
