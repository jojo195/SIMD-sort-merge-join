
#include <immintrin.h>
#include <stdint.h>

#include "../support/types.h"
#include "../support/config.h"
#include "SIMD_sort.h"

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

/* SIMD Sort */
__m512i bitonic_8_8u_vec(__m512i v) {

    /* Pairs: ([6,7], [4,5], [2,3], [0,1]) */
    /* Perm:  ( 6,  7,  4,  5,  2,  3,  0,  1) */
    __m512i perm0 = _mm512_shuffle_epi32(v, 0x4e);
    __m512i min0  = _mm512_min_epu64(v, perm0);
    __m512i v0    = _mm512_mask_max_epu64(min0, 0xaa, v, perm0);

    /* Pairs: ([4,7], [5,6], [0,3], [1,2]) */
    /* Perm:  ( 4,  5,  6,  7,  0,  1,  2,  3) */
    __m512i perm1 = _mm512_permutex_epi64(v0, 0x1b);
    __m512i min1  = _mm512_min_epu64(v0, perm1);
    __m512i v1    = _mm512_mask_max_epu64(min1, 0xcc, v0, perm1);

    /* Pairs: ([6,7], [4,5], [2,3], [0,1]) */
    /* Perm:  ( 6,  7,  4,  5,  2,  3,  0,  1) */
    __m512i perm2 = _mm512_shuffle_epi32(v1, 0x4e);
    __m512i min2  = _mm512_min_epu64(v1, perm2);
    __m512i v2    = _mm512_mask_max_epu64(min2, 0xaa, v1, perm2);

    /* Pairs: ([0,7], [1,6], [2,5], [3,4]) */
    /* Perm:  ( 0,  1,  2,  3,  4,  5,  6,  7) */
    __m512i _tmp0 = _mm512_shuffle_i64x2(v2, v2, 0x1b);
    __m512i perm3 = _mm512_shuffle_epi32(_tmp0, 0x4e);
    __m512i min3  = _mm512_min_epu64(v2, perm3);
    __m512i v3    = _mm512_mask_max_epu64(min3, 0xf0, v2, perm3);

    /* Pairs: ([5,7], [4,6], [1,3], [0,2]) */
    /* Perm:  ( 5,  4,  7,  6,  1,  0,  3,  2) */
    __m512i perm4 = _mm512_permutex_epi64(v3, 0x4e);
    __m512i min4  = _mm512_min_epu64(v3, perm4);
    __m512i v4    = _mm512_mask_max_epu64(min4, 0xcc, v3, perm4);

    /* Pairs: ([6,7], [4,5], [2,3], [0,1]) */
    /* Perm:  ( 6,  7,  4,  5,  2,  3,  0,  1) */
    __m512i perm5 = _mm512_shuffle_epi32(v4, 0x4e);
    __m512i min5  = _mm512_min_epu64(v4, perm5);
    __m512i v5    = _mm512_mask_max_epu64(min5, 0xaa, v4, perm5);

    return v5;
}

/* Wrapper For SIMD Sort */
void bitonic_8_8u(uint64_t *arr) {

    __m512i v = _mm512_load_si512((__m512i *)arr);

    v = bitonic_8_8u_vec(v);

    _mm512_store_si512((__m512i *)arr, v);
}

/**
 * sort 4 tuples per time 
 * while each kernel sort for 4 tuples
 * each tuple composed of one key and one rid
*/
void sort_8_tuples(tuple_t* tuples)
{
    bitonic_8_8u((uint64_t*)tuples);
}

// Recursive implementation of merge sort
// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
//@param tmp_L and tmp_R is demand to be CACHE_LINE aligned
//and exactly the same size as arr
void merge_tuple_SIMD(tuple_t* arr, tuple_t* tmp_L, tuple_t* tmp_R, int l, int m, int r)
{
	int i, j, k;
	int n1 = m - l + 1;
	int n2 = r - m;
	// Copy data to temp arrays
	// L[] and R[]
    memcpy(tmp_L, arr+l, n1*sizeof(tuple_t));
    memcpy(tmp_R, arr+m+1, n2*sizeof(tuple_t));
		
	// Merge the temp arrays back
	// into arr[l..r]
	// Initial index of first subarray
	i = 0;

	// Initial index of second subarray
	j = 0;

	// Initial index of merged subarray
	k = l;
	while (i < n1 && j < n2)
	{
		if (tmp_L[i].key <= tmp_R[j].key)
		{
			arr[k].key = tmp_L[i].key;
            arr[k].payload = tmp_L[i].payload;
			i++;
		}
		else
		{
			arr[k].key = tmp_R[j].key;
            arr[k].payload = tmp_R[j].payload;
			j++;
		}
		k++;
	}

	// Copy the remaining elements
	// of L[], if there are any
	while (i < n1) {
        arr[k].key = tmp_L[i].key;
		arr[k].payload = tmp_L[i].payload;
		i++;
		k++;
	}

	// Copy the remaining elements of
	// R[], if there are any
	while (j < n2)
	{
		arr[k].key = tmp_R[j].key;
        arr[k].payload = tmp_R[j].payload;
		j++;
		k++;
	}
}

// l is for left index and r is
// right index of the sub-array
// of arr to be sorted
void Mergesort_tuple_SIMD(tuple_t* arr, tuple_t* tmp_L, tuple_t* tmp_R, int l, int r)
{
    if((r-l+1)==8)
    {
        sort_4_tuples(arr+l);
        return;
    }
	if (l < r)
	{
		// Same as (l+r)/2, but avoids
		// overflow for large l and h
		int m = l + (r - l) / 2;

		// Sort first and second halves
		Mergesort_tuple_SIMD(arr, tmp_L, tmp_R, l, m);
		Mergesort_tuple_SIMD(arr, tmp_L, tmp_R, m + 1, r);

		merge_tuple_SIMD(arr, tmp_L, tmp_R, l, m, r);
	}
}

// UTILITY FUNCTIONS
// Function to print an array
void printArray(int A[], int size)
{
	int i;
	for (i = 0; i < size; i++)
		printf("%d ", A[i]);
	printf("\n");
}

void SIMD_relation_sort(relation_t* r)
{
    tuple_t* tmp_L = aligned_alloc(r->num_tuples*sizeof(tuple_t)/2, CACHE_LINE_SIZE);
	tuple_t* tmp_R = aligned_alloc(r->num_tuples*sizeof(tuple_t)/2, CACHE_LINE_SIZE);
    Mergesort_tuple_SIMD(r->tuples, tmp_L, tmp_R, 0, r->num_tuples - 1);
}