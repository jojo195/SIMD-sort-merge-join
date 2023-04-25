/**
* app.c
* Arithmetic Throughput Host Application Source File
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <assert.h>

#include "../support/common.h"
#include "../support/timer.h"
#include "../support/params.h"
#include "../support/types.h"
#include "../support/tuple_buffer.h"
#include"../support/config.h"

#include"generator.h"
#include"SIMD_sort.h"
#include"normal_sort.h"
#include <immintrin.h>

/** computes and returns the histogram size for join */
inline 
uint32_t 
get_hist_size(uint32_t relSize) __attribute__((always_inline));

//Canonical nested loop join
uint32_t Host_nested_loop_join(relation_t* R, relation_t* S, output_relation_t* output);

/**
 * compute output on the host
 * histogram join algrithem
 * @param R smaller relation for hash build
 * @param S lager relation for probe
 * @param tmpR buffer as the same size of R, allocated for hash
 * @param output output relation
 * @return number of tuples that match
*/
int32_t Host_histogram_join(relation_t* R, relation_t* S, relation_t* tmpR, output_relation_t* output);

uint32_t Host_quicksort_merge_join(relation_t* R, relation_t* S, output_relation_t* output);

uint32_t Host_SIMDsort_merge_join(relation_t* R, relation_t* S, output_relation_t* output);

uint32_t Host_mergesort_merge_join(relation_t* R, relation_t* S, output_relation_t* output);



// Main of the Host Application
int main(int argc, char **argv) {
    int rt=EXIT_SUCCESS;
    //create two relations for join
    relation_t R_rel; //const pointer
    relation_t S_rel;
    relation_t tmpR;
    rt=create_relation_fk(&R_rel, NUM_TUPLES_R, MAX_ID);
    rt|=create_relation_fk(&S_rel, NUM_TUPLES_S, MAX_ID);
    rt|=malloc_relation(&tmpR, NUM_TUPLES_R);
    output_relation_t* output_host=output_relation_init();
    if(rt!=EXIT_SUCCESS)
    {
        printf("ERROR: Failed to allocate memory\n");
        return rt;
    }
    struct Params p = input_params(argc, argv);

    // Timer declaration
    Timer timer;

    uint32_t checksum_cpu=0, checksum_dpu=0;

    // Loop over main kernel
    for(int rep = 0; rep < p.n_reps; rep++) {

        // Compute output on CPU (performance comparison and verification purposes)
        printf("Host_histogram_join...\n");
        start(&timer, 0, 1);
        checksum_cpu = Host_histogram_join(&R_rel, &S_rel, &tmpR, output_host);
        printf("cpu hash join match number: %d\n", checksum_cpu);
        stop(&timer, 0);
        printf("Host_nested_loop_join...\n");
        start(&timer, 1, 1);
        // checksum_cpu = Host_nested_loop_join(&R_rel, &S_rel, output_host);
        printf("cpu nested loop join match number: %d\n", checksum_cpu);
        stop(&timer, 1);
        printf("Host_quicksort_merge_join...\n");
        start(&timer, 2, 1);
        // checksum_cpu = Host_quicksort_merge_join(&R_rel, &S_rel, output_host);
        // checksum_cpu = Host_mergesort_merge_join(&R_rel, &S_rel, output_host);
        printf("cpu quicksort merge join match number: %d\n", checksum_cpu);
        stop(&timer, 2);
        printf("Host_SIMDsort_merge_join...\n");
        start(&timer, 3, 1);
        checksum_cpu = Host_SIMDsort_merge_join(&R_rel, &S_rel, output_host);
        printf("cpu SIMDsort merge join match number: %d\n", checksum_cpu);
        stop(&timer, 3);
    }
    // Print timing results
    printf("histogram_join ");
    print(&timer, 0, p.n_reps);
    printf("nested loop join ");
    print(&timer, 1, p.n_reps);
    printf("quicksort merge join ");
    print(&timer, 2, p.n_reps);
    printf("SIMDsort merge join ");
    print(&timer, 3, p.n_reps);
    printf("\n");
    // TODO: Check output
    bool status = true;

#if IS_MATERIRIALIZE
    output_relation_free(output_rel);
#endif
    return status ? 0 : -1;
}


inline 
uint32_t 
get_hist_size(uint32_t relSize) 
{
    NEXT_POW_2(relSize);
    relSize >>= 2;
    if(relSize < 4) relSize = 4; 
    return relSize;
}
uint32_t Host_nested_loop_join(relation_t* R, relation_t* S, output_relation_t* output)
{
    uint32_t match = 0;
    for(uint32_t R_index = 0; R_index < R->num_tuples; R_index++)
    {
        for(uint32_t S_index = 0; S_index < S->num_tuples; S_index++)
        {
            if(R->tuples[R_index].key == S->tuples[S_index].key)
            {
                match++;
            }
        }
    }
    return match;
}

// Compute output in the host
int32_t Host_histogram_join(relation_t* R, relation_t* S, relation_t* tmpR, output_relation_t* output)
{
    int32_t * restrict hist;
    const tuple_t * restrict const Rtuples = R->tuples;
    const uint32_t numR  = R->num_tuples;
    uint32_t       Nhist = get_hist_size(numR);
    const uint32_t MASK  = (Nhist-1) << NUM_RADIX_BITS;

    hist   = (int32_t*) calloc(Nhist+2, sizeof(int32_t));
    for( uint32_t i = 0; i < numR; i++ ) {

        uint32_t idx = HASH_BIT_MODULO(Rtuples[i].key, MASK, NUM_RADIX_BITS);
    // printf("key = %d, index = %d \n", Rtuples[i].key, idx);
        hist[idx+2] ++;
    }
    /* prefix sum on histogram */
    for( uint32_t i = 2, sum = 0; i <= Nhist+1; i++ ) {
        sum     += hist[i];
        hist[i]  = sum;
    }

    tuple_t * const tmpRtuples = tmpR->tuples;
    /* reorder tuples according to the prefix sum */
    for( uint32_t i = 0; i < numR; i++ ) {

        uint32_t idx = HASH_BIT_MODULO(Rtuples[i].key, MASK, NUM_RADIX_BITS) + 1;

        tmpRtuples[hist[idx]] = Rtuples[i];

        hist[idx] ++;
    }

    int32_t              match   = 0;
    const uint32_t        numS    = S->num_tuples;
    const tuple_t * const Stuples = S->tuples;
    /* now comes the probe phase, TODO: implement prefetching */
    for( uint32_t i = 0; i < numS; i++ ) {

        uint32_t idx = HASH_BIT_MODULO(Stuples[i].key, MASK, NUM_RADIX_BITS);

        int j = hist[idx], end = hist[idx+1];

        /* Scalar comparisons */
        for(; j < end; j++) {

            if(Stuples[i].key == tmpRtuples[j].key) {

                ++ match;
                /* TODO: we do not output results */
            }

        }
    }

    /* clean up */
    free(hist);

    return match;
}

uint32_t Host_quicksort_merge_join(relation_t* R, relation_t* S, output_relation_t* output)
{
    uint32_t matches = 0;
    Quicksort_relation(R);
    Quicksort_relation(S);
    int R_index = 0, S_index = 0;
    //a simple stack implement for multiple same key, store the bottom of stack
    int R_s_index = 0;
    while((R_index < NUM_TUPLES_R) && (S_index < NUM_TUPLES_S))
    {
        R_s_index = R_index;
        R_index++;
        //variable mark the completement of scanning of R relation
        char done = false;
        while(!done && R_index < NUM_TUPLES_R)
        {
            if(R->tuples[R_index].key == R->tuples[R_s_index].key)
            {
                R_index++;
            }
            else
            {
                done = true;
            }
        }
        while(S_index < NUM_TUPLES_S && S->tuples[S_index].key < R->tuples[R_s_index].key)
        {
            S_index++;
        }
        while(S_index < NUM_TUPLES_S && S->tuples[S_index].key == R->tuples[R_s_index].key)
        {
            int R_tmp_index = R_s_index;
            while(R_tmp_index != R_index)
            {
                matches++;
                R_tmp_index++;
            }
            S_index++;
        }
    }
    return matches;
}

uint32_t Host_mergesort_merge_join(relation_t* R, relation_t* S, output_relation_t* output)
{
    uint32_t matches = 0;
    Mergesort_relation(R);
    Mergesort_relation(S);
    int R_index = 0, S_index = 0;
    //a simple stack implement for multiple same key, store the bottom of stack
    int R_s_index = 0;
    while((R_index < NUM_TUPLES_R) && (S_index < NUM_TUPLES_S))
    {
        R_s_index = R_index;
        R_index++;
        //variable mark the completement of scanning of R relation
        char done = false;
        while(!done && R_index < NUM_TUPLES_R)
        {
            if(R->tuples[R_index].key == R->tuples[R_s_index].key)
            {
                R_index++;
            }
            else
            {
                done = true;
            }
        }
        while(S_index < NUM_TUPLES_S && S->tuples[S_index].key < R->tuples[R_s_index].key)
        {
            S_index++;
        }
        while(S_index < NUM_TUPLES_S && S->tuples[S_index].key == R->tuples[R_s_index].key)
        {
            int R_tmp_index = R_s_index;
            while(R_tmp_index != R_index)
            {
                matches++;
                R_tmp_index++;
            }
            S_index++;
        }
    }
    return matches;
}

uint32_t Host_SIMDsort_merge_join(relation_t* R, relation_t* S, output_relation_t* output)
{
    uint32_t matches = 0;
    SIMD_relation_sort(R);
    SIMD_relation_sort(S);
    int R_index = 0, S_index = 0;
    //a simple stack implement for multiple same key, store the bottom of stack
    int R_s_index = 0;
    while((R_index < NUM_TUPLES_R) && (S_index < NUM_TUPLES_S))
    {
        R_s_index = R_index;
        R_index++;
        //variable mark the completement of scanning of R relation
        char done = false;
        while(!done && R_index < NUM_TUPLES_R)
        {
            if(R->tuples[R_index].key == R->tuples[R_s_index].key)
            {
                R_index++;
            }
            else
            {
                done = true;
            }
        }
        while(S_index < NUM_TUPLES_S && S->tuples[S_index].key < R->tuples[R_s_index].key)
        {
            S_index++;
        }
        while(S_index < NUM_TUPLES_S && S->tuples[S_index].key == R->tuples[R_s_index].key)
        {
            int R_tmp_index = R_s_index;
            while(R_tmp_index != R_index)
            {
                matches++;
                R_tmp_index++;
            }
            S_index++;
        }
    }
    return matches;
}