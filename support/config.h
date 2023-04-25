#ifndef __CONFIG_H__
#define __CONFIG_H__


#ifndef NR_DPUS
#define NR_DPUS 1
#endif

//if generate materialized output
#ifndef IS_MATERIRIALIZE
#define IS_MATERIRIALIZE 0
#endif

/* below is character of test benchmark*/

#ifndef NUM_TUPLES_R
#define NUM_TUPLES_R 1200000
#endif
//Max tuples that one rank can hold is 320000
#ifndef NUM_TUPLES_S
#define NUM_TUPLES_S 1200000
#endif

//create microbenchmark for relation R and S
//following is the segment size
//R and S share a common max id
//temporerily set unique key for R relation
#ifndef MAX_ID_LOG2
#define MAX_ID_LOG2 16
#define MAX_ID (NUM_TUPLES_R >> 4)
#endif

/* below is character of hash calculation */

//radix join bits for hash calculation
#ifndef NUM_RADIX_BITS
#define NUM_RADIX_BITS 0
#endif

//First Pass hash bit should always be 0
#ifndef FIRST_PASS_HASH_BIT
#define FIRST_PASS_HASH_BIT 0
#endif

/**
 * for the multipass partitioning join algorithm
 * As the first pa
*/
#ifndef FIRST_PASS_BIT_LENGTH
#define FIRST_PASS_BIT_LENGTH 20
#endif

#ifndef SECOND_PASS_HASH_BIT
#define SECOND_PASS_HASH_BIT FIRST_PASS_BIT_LENGTH
#endif

#ifndef SECOND_PASS_BIT_LENGTH
#define SECOND_PASS_BIT_LENGTH 8
#endif

#ifndef CACHE_LINE_SIZE
#define CACHE_LINE_SIZE 64
#endif

//batch transfer size(Byte) between pim and host
#ifndef TRANSFER_BUFFER_SIZE
#define TRANSFER_BUFFER_SIZE (1 << 14)
#endif

//batch transfer number of tuples between pim and host
#ifndef TRANSFER_TUPLE_NUM
#ifdef KEY_8B
#define TRANSFER_TUPLE_NUM (TRANSFER_BUFFER_SIZE >> 4)
#else
#define TRANSFER_TUPLE_NUM (TRANSFER_BUFFER_SIZE >> 3)
#endif
#endif

//batch transfer of output relation between pim and host
//this attribute is alse used as chained buffer for output joined relation
#ifndef TRANSFER_OUTPUT_TUPLE_NUM
#ifdef KEY_8B
#define TRANSFER_OUTPUT_TUPLE_NUM (TRANSFER_BUFFER_SIZE >> 5)
#else
#define TRANSFER_OUTPUT_TUPLE_NUM (TRANSFER_BUFFER_SIZE >> 4)
#endif
#endif

//size of tuple (Bytes)
#ifdef KEY_8B
#define TUPLE_SIZE_LOG2 4
#define TUPLE_SIZE (1 << TUPLE_SIZE_LOG2)
#else
#define TUPLE_SIZE_LOG2 3
#define TUPLE_SIZE (1 << TUPLE_SIZE_LOG2)
#endif

//sizeof R and S (Bytes)
#define R_SIZE (NUM_TUPLES_R << TUPLE_SIZE_LOG2)
#define S_SIZE (NUM_TUPLES_S << TUPLE_SIZE_LOG2)

//transfer between mram and wram has to be 8 bytes aligned
#define WRAM_ALIGN 8

#define MAX_NR_TASKLET 24

//From MRAM to WRAM batch 
#ifndef MRAM2WRAM_BATCH
#define MRAM2WRAM_BATCH 128
#endif


#endif