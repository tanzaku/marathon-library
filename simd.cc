
#include <cstdint>

#include <iostream>
#include <chrono>

#include <immintrin.h>

const size_t N = 256 * 1024 * 1024;
const size_t ALIGNMENT = 32;

// /mnt/d/d/work/src/competitive_programming/c++/lib
// g++ simd.cc -std=c++11 -mavx2

__attribute__((aligned(ALIGNMENT))) std::uint8_t data1[N];
__attribute__((aligned(ALIGNMENT))) std::uint8_t data2[N];
__attribute__((aligned(ALIGNMENT))) std::uint8_t data[N];

void test_add() {
    for (int i = 0; i < N; i++) {
        data1[i] = i & 0x7F;
        data2[i] = (13 * i) & 0x7F;
    }

    auto start = std::chrono::system_clock::now();
#if 1
    for (int i = 0; i < N; i++) {
        data[i] = data1[i] + data2[i];
    }
#else
    __m256i* a = (__m256i*)data1;
    __m256i* b = (__m256i*)data2;
    __m256i* c = (__m256i*)data;
    for (int i = 0; i < N / 32; i++) {
        // *c = _mm256_add_epi8(*a, *b);
        // *c = _mm256_adds_epi8(*a, *b);
        *c = _mm256_adds_epu8(*a, *b);
        a++, b++, c++;
    }
#endif
    auto end = std::chrono::system_clock::now();
    auto dur = end - start;
    auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

    std::uint32_t sum = 0;
    for (int i = 0; i < N; i++) {
        sum += data[i];
    }

    std::cout << msec << " milli sec" << std::endl;
    std::cout << (int)sum << " : sum" << std::endl;
}

void test_shuffle() {
    __m256i a = _mm256_set_epi32(
        0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00,
        0x12345678, 0x9ABCDEF0, 0x13579BDF, 0x2468ACE0);
    __m256i b = _mm256_set_epi8(
        3, 2, 1, 0, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
    __m256i c = _mm256_shuffle_epi8(a, b);

    for (int i = 0; i < 32; i++) {
        // std::cout << i << ": " << std::hex << (int)((std::uint8_t*)&a)[i] << std::endl;
        // std::cout << i << ": " << std::hex << (int)((std::uint8_t*)&b)[i] << std::endl;
        std::cout << i << ": " << std::hex << (int)((std::uint8_t*)&c)[i] << std::endl;
    }
}

void test_shift() {
    __m256i a = _mm256_set_epi32(
        0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00,
        0x12345678, 0x9ABCDEF0, 0x13579BDF, 0x2468ACE0);
    // __m256i a = _mm256_set_epi32(
    //     0x10000000, 0x00000000, 0x00000000, 0x00000000,
    //     0x00000000, 0x00000000, 0x00000000, 0x00000002);
    __m256i b = _mm256_set_epi32(
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000001);
    // __m256i b = _mm256_set1_epi32(1);
    __m256i c = _mm256_srlv_epi32(a, b);
    // __m256i c = _mm256_srlv_epi64(a, b);

    for (int i = 0; i < 32; i++) {
        // std::cout << i << ": " << std::hex << (int)((std::uint8_t*)&a)[i] << std::endl;
        // std::cout << i << ": " << std::hex << (int)((std::uint8_t*)&b)[i] << std::endl;
        std::cout << i << ": " << std::hex << (int)((std::uint8_t*)&c)[i] << std::endl;
    }
}

void test_doit() {
    constexpr std::uint32_t dc_32 = 0x00000000;
    const __m128i dc = _mm_set_epi32(dc_32, dc_32, dc_32, dc_32);

    // 
    __m128i mask_value = _mm_set_epi8(3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0);
    __m128i mask_value_shuffle = _mm_set_epi8(15, 11, 7, 3, 14, 10, 6, 2, 13, 9, 5, 1, 12, 8, 4, 0);
    __m128i mask_value_shuffled = _mm_shuffle_epi8(mask_value, mask_value_shuffle);

    // __m128i a = _mm_set_epi32(
    //     0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00);
    __m128i a = _mm_set1_epi32(0x11223344);     // a = *(uint32*)img
    __m128i b = _mm_set_epi8(3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0);
    __m128i c = _mm_set_epi8(
        0xC0, 0xC0, 0xC0, 0xC0,
        0x30, 0x30, 0x30, 0x30,
        0x0C, 0x0C, 0x0C, 0x0C,
        0x03, 0x03, 0x03, 0x03);
    __m128i d = _mm_set_epi32(6, 4, 2, 0);
    __m128i z = _mm_shuffle_epi8(a, b);
    __m128i y = _mm_and_si128(c, z);         // y = [0, 4, 8, C, 1, 5, 9, D, 2, 6, A, E, 3, 7, B, F]
    __m128i x = _mm_srlv_epi32(y, d);
    __m128i w = _mm_slli_epi64(x, 2);
    __m128i v = _mm_add_epi8(w, mask_value_shuffled);   // v = (uint8 [img*4+mask]) * 16
    // maskx                     0x00, 0x80, 0x80, 0x80 ?
    __m128i mask1 = _mm_set_epi8(0x80, 0x80, 0x80, 0x00, 0x80, 0x80, 0x80, 0x01, 0x80, 0x80, 0x80, 0x02, 0x80, 0x80, 0x80, 0x03);
    __m128i mask2 = _mm_set_epi8(0x80, 0x80, 0x80, 0x04, 0x80, 0x80, 0x80, 0x05, 0x80, 0x80, 0x80, 0x06, 0x80, 0x80, 0x80, 0x07);
    __m128i mask3 = _mm_set_epi8(0x80, 0x80, 0x80, 0x08, 0x80, 0x80, 0x80, 0x09, 0x80, 0x80, 0x80, 0x0A, 0x80, 0x80, 0x80, 0x0B);
    __m128i mask4 = _mm_set_epi8(0x80, 0x80, 0x80, 0x0C, 0x80, 0x80, 0x80, 0x0D, 0x80, 0x80, 0x80, 0x0E, 0x80, 0x80, 0x80, 0x0F);
    __m128i and_dc = _mm_set1_epi32(0x00000003);
    __m128i u1 = _mm_shuffle_epi8(v, mask1);             // u = (uint32 [img*4+mask]) * 4
    __m128i t1 = _mm_slli_epi64(u1, 2);                   // u = (uint32 [img*4+mask]) * 4
    __m128i s1 = _mm_srlv_epi32(dc, t1);
    __m128i r1 = _mm_and_si128(s1, and_dc);
    __m128i q1 = _mm_slli_epi64(r1, 6);
    
    __m128i u2 = _mm_shuffle_epi8(v, mask2);             // u = (uint32 [img*4+mask]) * 4
    __m128i t2 = _mm_slli_epi64(u2, 2);                   // u = (uint32 [img*4+mask]) * 4
    __m128i s2 = _mm_srlv_epi32(dc, t2);
    __m128i r2 = _mm_and_si128(s2, and_dc);
    __m128i q2 = _mm_slli_epi64(r2, 4);
    
    __m128i u3 = _mm_shuffle_epi8(v, mask3);             // u = (uint32 [img*4+mask]) * 4
    __m128i t3 = _mm_slli_epi64(u3, 2);                   // u = (uint32 [img*4+mask]) * 4
    __m128i s3 = _mm_srlv_epi32(dc, t3);
    __m128i r3 = _mm_and_si128(s3, and_dc);
    __m128i q3 = _mm_slli_epi64(r3, 2);
    
    __m128i u4 = _mm_shuffle_epi8(v, mask4);             // u = (uint32 [img*4+mask]) * 4
    __m128i t4 = _mm_slli_epi64(u4, 2);                   // u = (uint32 [img*4+mask]) * 4
    __m128i s4 = _mm_srlv_epi32(dc, t4);
    __m128i r4 = _mm_and_si128(s4, and_dc);
    // __m128i q4 = _mm_slli_epi64(r4, 0);
    __m128i q4 = r4;

    __m128i res12 = _mm_add_epi32(r1, r2);
    __m128i res34 = _mm_add_epi32(r3, r4);
    __m128i res = _mm_add_epi32(res12, res34);

    // for (int i = 0; i < width; i++) {
    //     std::uint32_t v =
    //         (dc >> (((*img >> 4 & 0x0C) + *mask++) << 1) & 0x03) << 6 |
    //         (dc >> (((*img >> 2 & 0x0C) + *mask++) << 1) & 0x03) << 4 |
    //         (dc >> (((*img >> 0 & 0x0C) + *mask++) << 1) & 0x03) << 2 |
    //         (dc >> (((*img << 2 & 0x0C) + *mask++) << 1) & 0x03) << 0;
    // }

    for (int i = 0; i < 16; i++) {
        // std::cout << i << ": " << std::hex << (int)((std::uint8_t*)&a)[i] << std::endl;
        // std::cout << i << ": " << std::hex << (int)((std::uint8_t*)&b)[i] << std::endl;
        // std::cout << i << ": " << std::hex << (int)((std::uint8_t*)&c)[i] << std::endl;
        // std::cout << i << ": " << std::hex << (int)((std::uint8_t*)&d)[i] << std::endl;
        // std::cout << i << ": " << std::hex << (int)((std::uint8_t*)&z)[i] << std::endl;
        // std::cout << i << ": " << std::hex << (int)((std::uint8_t*)&y)[i] << std::endl;
        // std::cout << i << ": " << std::hex << (int)((std::uint8_t*)&x)[i] << std::endl;
        std::cout << i << ": " << std::hex << (int)((std::uint8_t*)&w)[i] << std::endl;
    }
}

int main() {
    // test_add();
    // test_shuffle();
    // test_shift();
    test_doit();
    return 0;
}
