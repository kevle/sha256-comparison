# SHA256 Hashing Performance Comparison

This repository benchmarks various SHA256 hashing implementations, comparing their performance across different data sizes and thread counts using the Google benchmark library.

SHA256 is a hashing algorithm widely used for file integrity checks, Merkle tree construction and more. For some use cases, it is desirable to quickly compute a SHA256 value for rather small chunks of bytes. 
Modern processors often support instructions to accelerate SHA256 hashing.

The following SHA256 hashing implementations are benchmarked:

| Name                                               | Uses intrinsics | References                                                                                                 |
| -------------------------------------------------- | --------------- | ---------------------------------------------------------------------------------------------------------- |
| zedwood                                            | No              | [zedwood](http://www.zedwood.com/article/cpp-sha256-function), [Olivier Gay](https://github.com/ogay/sha2) |
| bcrypt                                             | Yes             | [Win32 API bcrypt.h](https://learn.microsoft.com/en-us/windows/win32/api/bcrypt/)                          |
| bitcoin                                            | Yes             | [bitcoin source code](https://github.com/bitcoin/bitcoin/tree/master/src/crypto)                           |
| openssl deprecated                                 | Yes             | [OpenSSL API, deprecated](https://docs.openssl.org/master/man3/SHA256_Init/)                               |
| openssl oneshot                                    | Yes             | [OpenSSL API](https://docs.openssl.org/master/man3/SHA256_Init/#synopsis)                                  |
| openssl (EVP digest)                               | Yes             | [OpenSSL API](https://docs.openssl.org/master/man7/ossl-guide-libcrypto-introduction)                      |
| openssl global (EVP digest, single explicit fetch) | Yes             | [OpenSSL API](https://docs.openssl.org/master/man7/ossl-guide-libcrypto-introduction)                      |

# Results

## AMD Ryzen 7 5800X3D on Windows

Compiled using Clang 17.0.6

![Benchmark results 256 byte chunks](results/5800X3D/output_256.png "5800X3D 256 byte chunks")
![Benchmark results 512 byte chunks](results/5800X3D/output_512.png "5800X3D 512 byte chunks")
![Benchmark results 4096 byte chunks](results/5800X3D/output_4096.png "5800X3D 4096 byte chunks")
![Benchmark results 32768 byte chunks](results/5800X3D/output_32768.png "5800X3D 32768 byte chunks")

## AMD Ryzen 7 7840HS on Linux

Compiled using GCC 14.2.1

![Benchmark results 256 byte chunks](results/7840HS/output_256.png "7840HS 256 byte chunks")
![Benchmark results 512 byte chunks](results/7840HS/output_512.png "7840HS 512 byte chunks")
![Benchmark results 4096 byte chunks](results/7840HS/output_4096.png "7840HS 4096 byte chunks")
![Benchmark results 32768 byte chunks](results/7840HS/output_32768.png "7840HS 32768 byte chunks")

## 2x Intel Xeon Platinum 8468 on Linux

Compiled using Clang 18.1.8

![Benchmark results 256 byte chunks](results/8468/output_256.png "8468 256 byte chunks")
![Benchmark results 512 byte chunks](results/8468/output_512.png "8468 512 byte chunks")
![Benchmark results 4096 byte chunks](results/8468/output_4096.png "8468 4096 byte chunks")
![Benchmark results 32768 byte chunks](results/8468/output_32768.png "8468 32768 byte chunks")

# Key Takeaways

Always make sure to use an implementation that supports intrinsics when they are available.

If you require high performance for small chunks of data, use an implementation supporting intrinsics and without heap allocation or other shared global state.

For chunks with at least around 32768 bytes, you can get excellent performance by using the Win32 API or any OpenSSL API.

# TODO

Potentially interesting SHA256 implementations to try

https://github.com/minio/sha256-simd

https://github.com/intel/intel-ipsec-mb



