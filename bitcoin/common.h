// Copyright (c) 2014-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CRYPTO_COMMON_H
#define BITCOIN_CRYPTO_COMMON_H

#include <stdint.h>
#include <string.h>

uint16_t static inline ReadLE16(const unsigned char* ptr)
{
    uint16_t x;
    memcpy(&x, ptr, 2);
    return x;
}

uint32_t static inline ReadLE32(const unsigned char* ptr)
{
    uint32_t x;
    memcpy(&x, ptr, 4);
    return x;
}

uint64_t static inline ReadLE64(const unsigned char* ptr)
{
    uint64_t x;
    memcpy(&x, ptr, 8);
    return x;
}

void static inline WriteLE16(unsigned char* ptr, uint16_t x)
{
    uint16_t v = x;
    memcpy(ptr, &v, 2);
}

void static inline WriteLE32(unsigned char* ptr, uint32_t x)
{
    uint32_t v = x;
    memcpy(ptr, &v, 4);
}

void static inline WriteLE64(unsigned char* ptr, uint64_t x)
{
    uint64_t v = x;
    memcpy(ptr, &v, 8);
}

template<typename Int>
Int static inline ReadBE(const unsigned char* ptr)
{
    constexpr int num_bytes = sizeof(Int);
    Int out = 0;
    for (int i = 0; i < num_bytes; ++i) {
        out |= (Int(ptr[num_bytes - 1 - i]) << (i * 8));
    }
    return out;
}

uint16_t static inline ReadBE16(const unsigned char* ptr)
{
    return ReadBE<uint16_t>(ptr);
}

uint32_t static inline ReadBE32(const unsigned char* ptr)
{
    return ReadBE<uint32_t>(ptr);
}

uint64_t static inline ReadBE64(const unsigned char* ptr)
{
    return ReadBE<uint64_t>(ptr);
}

void static inline WriteBE32(unsigned char* ptr, uint32_t x)
{
    x = ReadBE<uint32_t>(reinterpret_cast<const unsigned char*>(&x));
    memcpy(ptr, &x, 4);
}

void static inline WriteBE64(unsigned char* ptr, uint64_t x)
{
    x = ReadBE<uint64_t>(reinterpret_cast<const unsigned char*>(&x));
    memcpy(ptr, &x, 8);
}

#endif // BITCOIN_CRYPTO_COMMON_H
