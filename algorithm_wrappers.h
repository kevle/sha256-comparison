#pragma once

#include <array>
#include <cassert>
#include <limits>
#include <memory>
#include <vector>

// zedwood
#include "zedwood/sha256.h"

#ifdef BITCOIN_IMPL
// bitcoin
#include "bitcoin/sha256.h"
#endif

// OpenSSL
#include <openssl/evp.h>
#include <openssl/sha.h>

#ifdef USE_NSS
// NSS
#include <hasht.h>
#include <nsslowhash.h>
#endif

// Bcrypt
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <ntstatus.h>
//
#include <Bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#endif

struct sha256_dummy
{
  void add_bytes(const unsigned char *, std::size_t) {}
  std::array<unsigned char, 32> digest() { return {}; }
};

struct sha256_zedwood
{
  zedwood::SHA256 ctx;
  sha256_zedwood() { ctx.init(); }
  inline void add_bytes(const unsigned char *bytes, std::size_t num)
  {
    constexpr std::size_t max_bytes =
        (std::numeric_limits<unsigned int>::max)() / 2;
    unsigned int size = 0;
    for (std::size_t i = 0; i < num; i += size, bytes += size)
    {
      size = static_cast<unsigned int>((std::min)(max_bytes, num - i));
      ctx.update(bytes, size);
    }
  }
  std::array<unsigned char, 32> digest()
  {
    std::array<unsigned char, 32> tmp;
    ctx.final(tmp.data());
    return tmp;
  }
};

struct sha256_openssl_deprecated
{
  SHA256_CTX ctx = {};
  sha256_openssl_deprecated() { SHA256_Init(&ctx); }
  void add_bytes(const unsigned char *bytes, std::size_t num)
  {
    static_assert((std::numeric_limits<size_t>::max)() ==
                      (std::numeric_limits<std::size_t>::max)(),
                  "Incompatible size_t");
    SHA256_Update(&ctx, bytes, num);
  }
  std::array<unsigned char, 32> digest()
  {
    std::array<unsigned char, 32> tmp;
    SHA256_Final(tmp.data(), &ctx);
    return tmp;
  }
};

struct openssl_evp_destroyer
{
  void operator()(EVP_MD_CTX *ctx) const { EVP_MD_CTX_destroy(ctx); }
};

struct openssl_md_destroyer
{
  void operator()(EVP_MD *md) const { EVP_MD_free(md); }
};

struct sha256_openssl
{
  std::unique_ptr<EVP_MD_CTX, openssl_evp_destroyer> ctx;
  const std::unique_ptr<EVP_MD, openssl_md_destroyer> md;

  sha256_openssl() : ctx(EVP_MD_CTX_create()), md(EVP_MD_fetch(NULL, "SHA256", NULL))
  {
    assert(static_cast<bool>(ctx));
    EVP_DigestInit_ex(ctx.get(), md.get(), NULL);
  }

  void add_bytes(const unsigned char *bytes, std::size_t num)
  {
    static_assert((std::numeric_limits<size_t>::max)() ==
                      (std::numeric_limits<std::size_t>::max)(),
                  "Incompatible size_t");
    EVP_DigestUpdate(ctx.get(), bytes, num);
  }
  std::array<unsigned char, 32> digest()
  {
    std::array<unsigned char, 32> tmp;
    EVP_DigestFinal_ex(ctx.get(), tmp.data(), nullptr);
    return tmp;
  }
};

static std::unique_ptr<EVP_MD, openssl_md_destroyer> global_md;

struct sha256_openssl_global
{
  std::unique_ptr<EVP_MD_CTX, openssl_evp_destroyer> ctx;

  sha256_openssl_global() : ctx(EVP_MD_CTX_create())
  {
    assert(static_cast<bool>(ctx));
    EVP_DigestInit_ex(ctx.get(), global_md.get(), NULL);
  }

  void add_bytes(const unsigned char *bytes, std::size_t num)
  {
    static_assert((std::numeric_limits<size_t>::max)() ==
                      (std::numeric_limits<std::size_t>::max)(),
                  "Incompatible size_t");
    EVP_DigestUpdate(ctx.get(), bytes, num);
  }
  std::array<unsigned char, 32> digest()
  {
    std::array<unsigned char, 32> tmp;
    EVP_DigestFinal_ex(ctx.get(), tmp.data(), nullptr);
    return tmp;
  }
};

struct sha256_openssl_oneshot
{
  std::array<unsigned char, 32> digest_data;
  void add_bytes(const unsigned char *bytes, std::size_t num)
  {
    SHA256(bytes, num, digest_data.data());
  }
  std::array<unsigned char, 32> digest()
  {
    return digest_data;
  }
};

#ifdef _WIN32
struct bcrypt_alg_destroyer
{
  void operator()(BCRYPT_ALG_HANDLE ctx) const
  {
    BCryptCloseAlgorithmProvider(ctx, 0);
  }
};

struct bcrypt_hash_destroyer
{
  void operator()(BCRYPT_HASH_HANDLE ctx) const { BCryptDestroyHash(ctx); }
};

struct sha256_bcrypt
{
  std::unique_ptr<void, bcrypt_alg_destroyer> alg;
  std::unique_ptr<void, bcrypt_hash_destroyer> ctx;

  sha256_bcrypt()
  {
    {
      BCRYPT_ALG_HANDLE handle;
      auto ret = BCryptOpenAlgorithmProvider(&handle, BCRYPT_SHA256_ALGORITHM,
                                             NULL, 0);
      (void)ret;
      assert(ret == STATUS_SUCCESS);
      alg.reset(handle);
    }
    {
      BCRYPT_HASH_HANDLE handle;
      auto ret = BCryptCreateHash(alg.get(), &handle, NULL, 0, NULL, 0, 0);
      (void)ret;
      assert(ret == STATUS_SUCCESS);
      ctx.reset(handle);
    }
  }

  void add_bytes(const unsigned char *bytes, std::size_t num)
  {
    constexpr std::size_t max_bytes = (std::numeric_limits<ULONG>::max)();
    unsigned int size = 0;
    for (std::size_t i = 0; i < num; i += size, bytes += size)
    {
      size = static_cast<ULONG>((std::min)(max_bytes, num - i));
      auto ret = BCryptHashData(ctx.get(), const_cast<unsigned char *>(bytes), size, 0);
      (void)ret;
      assert(ret == STATUS_SUCCESS);
    }
  }
  std::array<unsigned char, 32> digest()
  {
    std::array<unsigned char, 32> tmp;
    auto ret = BCryptFinishHash(ctx.get(), tmp.data(), tmp.size(), 0);
    (void)ret;
    assert(ret == STATUS_SUCCESS);
    return tmp;
  }
};
#endif

#ifdef BITCOIN_IMPL
struct sha256_bitcoin
{
  CSHA256 ctx;

  void add_bytes(const unsigned char *bytes, std::size_t num)
  {
    ctx.Write(bytes, num);
  }
  std::array<unsigned char, 32> digest()
  {
    std::array<unsigned char, 32> tmp;
    ctx.Finalize(tmp.data());
    return tmp;
  }
};
#endif
