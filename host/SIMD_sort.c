
#include <immintrin.h>
#include <stdint.h>

#include "SIMD_sort.h"
#include "../support/types.h"

/* SIMD Sort */
__m256i bitonic_8_4u_vec(__m256i v) {

    /* Pairs: ([6,7], [4,5], [2,3], [0,1]) */
    /* Perm:  ( 6,  7,  4,  5,  2,  3,  0,  1) */
    __m256i perm0 = _mm256_shuffle_epi32(v, 0xb1);
    __m256i min0  = _mm256_min_epu32(v, perm0);
    __m256i max0  = _mm256_max_epu32(v, perm0);
    __m256i v0    = _mm256_blend_epi32(max0, min0, 0x55);

    /* Pairs: ([4,7], [5,6], [0,3], [1,2]) */
    /* Perm:  ( 4,  5,  6,  7,  0,  1,  2,  3) */
    __m256i perm1 = _mm256_shuffle_epi32(v0, 0x1b);
    __m256i min1  = _mm256_min_epu32(v0, perm1);
    __m256i max1  = _mm256_max_epu32(v0, perm1);

    /* Pairs: ([6,7], [4,5], [2,3], [0,1]) */
    /* Perm:  ( 6,  7,  4,  5,  2,  3,  0,  1) */
    /* Reordering Permutate and Blend for shorted dependency chain */
    __m256i perm2 = _mm256_castps_si256(_mm256_shuffle_ps(
        _mm256_castsi256_ps(min1), _mm256_castsi256_ps(max1), 0xb1));
    __m256i v1    = _mm256_blend_epi32(max1, min1, 0x33);

    __m256i min2 = _mm256_min_epu32(v1, perm2);
    __m256i max2 = _mm256_max_epu32(v1, perm2);

    /* Pairs: ([0,7], [1,6], [2,5], [3,4]) */
    /* Perm:  ( 0,  1,  2,  3,  4,  5,  6,  7) */
    /* Reordering Permutate and Blend for shorted dependency chain */
    __m256i perm3 = _mm256_permutex2var_epi32(max2, _mm256_set_epi32(8, 1, 10, 3, 12, 5, 14, 7), min2);
    // __m256i perm3 = max2;
    __m256i v2 = _mm256_blend_epi32(max2, min2, 0x55);

    __m256i min3 = _mm256_min_epu32(v2, perm3);
    __m256i max3 = _mm256_max_epu32(v2, perm3);
    __m256i v3   = _mm256_blend_epi32(max3, min3, 0xf);

    /* Pairs: ([5,7], [4,6], [1,3], [0,2]) */
    /* Perm:  ( 5,  4,  7,  6,  1,  0,  3,  2) */
    __m256i perm4 = _mm256_shuffle_epi32(v3, 0x4e);
    __m256i min4  = _mm256_min_epu32(v3, perm4);
    __m256i max4  = _mm256_max_epu32(v3, perm4);

    /* Pairs: ([6,7], [4,5], [2,3], [0,1]) */
    /* Perm:  ( 6,  7,  4,  5,  2,  3,  0,  1) */
    /* Reordering Permutate and Blend for shorted dependency chain */
    __m256i perm5 = _mm256_castps_si256(_mm256_shuffle_ps(
        _mm256_castsi256_ps(min4), _mm256_castsi256_ps(max4), 0xb1));
    __m256i v4    = _mm256_blend_epi32(max4, min4, 0x33);

    __m256i min5 = _mm256_min_epu32(v4, perm5);
    __m256i max5 = _mm256_max_epu32(v4, perm5);
    __m256i v5   = _mm256_blend_epi32(max5, min5, 0x55);

    return v5;
}

/* Wrapper For SIMD Sort */
void bitonic_8_4u(uint32_t * const arr) {

    __m256i v = _mm256_load_si256((__m256i *)arr);

    v = bitonic_8_4u_vec(v);

    _mm256_store_si256((__m256i *)arr, v);
}

/* SIMD Sort */
__m256i bitonic_4_8u_vec(__m256i v) {

    /* Pairs: ([2,3], [0,1]) */
    /* Perm:  ( 2,  3,  0,  1) */
    __m256i perm0 = _mm256_shuffle_epi32(v, 0x4e);
    __m256i min0  = _mm256_min_epi64(v, perm0);
    __m256i max0  = _mm256_max_epu64(v, perm0);
    __m256i v0    = _mm256_blend_epi32(max0, min0, 0x33);

    /* Pairs: ([0,3], [1,2]) */
    /* Perm:  ( 0,  1,  2,  3) */
    // __m256i perm1 = v0;
    __m256i perm1 = _mm256_permute4x64_epi64(v0, 0x1b);
    __m256i min1  = _mm256_min_epu64(v0, perm1);
    __m256i max1  = _mm256_max_epu64(v0, perm1);
    __m256i v1    = _mm256_blend_epi32(max1, min1, 0xf);

    /* Pairs: ([2,3], [0,1]) */
    /* Perm:  ( 2,  3,  0,  1) */
    __m256i perm2 = _mm256_shuffle_epi32(v1, 0x4e);
    __m256i min2  = _mm256_min_epu64(v1, perm2);
    __m256i max2  = _mm256_max_epu64(v1, perm2);
    __m256i v2    = _mm256_blend_epi32(max2, min2, 0x33);

    return v2;
}

/* Wrapper For SIMD Sort */
void bitonic_4_8u(uint64_t * const arr) {

    __m256i v = _mm256_load_si256((__m256i *)arr);

    v = bitonic_4_8u_vec(v);

    _mm256_store_si256((__m256i *)arr, v);
}

/**
 * sort 4 tuples per time 
 * while each kernel sort for 4 tuples
 * each tuple composed of one key and one rid
*/
void sort_4_tuples(tuple_t* tuples)
{
    bitonic_4_8u((uint64_t*)tuples);
}

void relation_sort(relation_t* origin, relation_t* output)
{

}