#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>

/** Structures used by both the host and the dpu to communicate information 
 * @name 
 * 
 */ 
typedef struct {
    uint32_t R_num_tuples;
	uint32_t S_num_tuples;
} dpu_arguments_t;

typedef struct {
    uint64_t cycles;
} dpu_results_t;

// Transfer size between MRAM and WRAM
#ifdef BL
// Transfer size between MRAM and WRAM
#define BLOCK_SIZE_LOG2 BL
#define BLOCK_SIZE (1 << BLOCK_SIZE_LOG2)
#else
#define BLOCK_SIZE_LOG2 8
#define BLOCK_SIZE (1 << BLOCK_SIZE_LOG2)
#define BL BLOCK_SIZE_LOG2
#endif

//cut down A into multiple times of 1 << B_LOG2 
#define FLOOR(A, B_LOG2) (((A) >> (B_LOG2)) << (B_LOG2))

// Data type
#ifdef UINT32
#define T uint32_t
#elif UINT64
#define T uint64_t
#elif INT32
#define T int32_t
#elif INT64
#define T int64_t
#elif FLOAT
#define T float
#elif DOUBLE
#define T double
#endif

#define PERF 0 // Use perfcounters?
#define PRINT 1

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#ifndef NEXT_POW_2
/** 
 *  compute the next number, greater than or equal to 32-bit unsigned v.
 *  taken from "bit twiddling hacks":
 *  http://graphics.stanford.edu/~seander/bithacks.html
 */
#define NEXT_POW_2(V)                           \
    do {                                        \
        V--;                                    \
        V |= V >> 1;                            \
        V |= V >> 2;                            \
        V |= V >> 4;                            \
        V |= V >> 8;                            \
        V |= V >> 16;                           \
        V++;                                    \
    } while(0)
#endif

//for hash join implementation
#define HASH_BIT_MODULO(K, MASK, NBITS) (((K) & MASK) >> NBITS)

#endif
