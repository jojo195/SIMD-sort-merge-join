/* @version $Id: generator.c 4546 2013-12-07 13:56:09Z bcagri $ */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <assert.h>             /* assert() */
#include <sched.h>              /* CPU_ZERO, CPU_SET */
#include <pthread.h>            /* pthread_attr_setaffinity_np */
#include <stdio.h>              /* perror */
#include <stdlib.h>             /* posix_memalign */
#include <math.h>               /* fmod, pow */
#include <time.h>               /* time() */
#include <unistd.h>             /* getpagesize() */
#include <string.h>             /* memcpy() */

#include "generator.h"          /* create_relation_*() */

#define CACHE_LINE_SIZE 64

/* return a random number in range [0,N] */
#define RAND_RANGE(N) ((double)rand() / ((double)RAND_MAX + 1) * (N))
#define RAND_RANGE48(N,STATE) ((double)nrand48(STATE)/((double)RAND_MAX+1)*(N))
#define MALLOC(SZ) alloc_aligned(SZ) /*malloc(SZ+RELATION_PADDING)*/ 
#define FREE(X,SZ) free(X)

#ifndef BARRIER_ARRIVE
/** barrier wait macro */
#define BARRIER_ARRIVE(B,RV)                            \
    RV = pthread_barrier_wait(B);                       \
    if(RV !=0 && RV != PTHREAD_BARRIER_SERIAL_THREAD){  \
        printf("Couldn't wait on barrier\n");           \
        exit(EXIT_FAILURE);                             \
    }
#endif

/* Uncomment the following to persist input relations to disk. */
/* #define PERSIST_RELATIONS 1 */

int nthreads;

static int seeded = 0;
static unsigned int seedValue;

void * alloc_aligned(size_t size)
{
    void * ret;
    int rv;
    rv = posix_memalign((void**)&ret, CACHE_LINE_SIZE, size);
    if (rv) { 
        perror("[ERROR] alloc_aligned() failed: out of memory");
        return 0; 
    }
    return ret;
}

void seed_generator(unsigned int seed) 
{
    srand(seed);
    seedValue = seed;
    seeded = 1;
}

/** Check wheter seeded, if not seed the generator with current time */
static void
check_seed()
{
    if(!seeded) {
        seedValue = time(NULL);
        srand(seedValue);
        seeded = 1;
    }
}


/** 
 * Shuffle tuples of the relation using Knuth shuffle.
 * 
 * @param relation 
 */
void knuth_shuffle(relation_t * relation)
{
    int i;
    for (i = relation->num_tuples - 1; i > 0; i--) {
        int64_t  j              = RAND_RANGE(i);
        intkey_t tmp            = relation->tuples[i].key;
        relation->tuples[i].key = relation->tuples[j].key;
        relation->tuples[j].key = tmp;
    }
}

void knuth_shuffle48(relation_t * relation, unsigned short * state)
{
    int i;
    for (i = relation->num_tuples - 1; i > 0; i--) {
        int64_t  j              = RAND_RANGE48(i, state);
        intkey_t tmp            = relation->tuples[i].key;
        relation->tuples[i].key = relation->tuples[j].key;
        relation->tuples[j].key = tmp;
    }
}

/**
 * Generate unique tuple IDs with Knuth shuffling
 * relation must have been allocated
 */
void random_unique_gen(relation_t *rel) 
{
    uint64_t i;

    for (i = 0; i < rel->num_tuples; i++) {
        rel->tuples[i].key = (i+1);
        rel->tuples[i].payload = i;
    }
    /* randomly shuffle elements */
    knuth_shuffle(rel);
}

/**
 * Generate tuple IDs -> random distribution
 * relation must have been allocated
 */
void
random_gen(relation_t *rel, const int64_t maxid) 
{
    uint64_t i;

    for (i = 0; i < rel->num_tuples; i++) {
        rel->tuples[i].key     = RAND_RANGE(maxid);
        rel->tuples[i].payload = i;
    }
}

/** generate relation that have the following feature
 * number of tuples == @param num_tuples
 * number of segments == @param num_tupls / @param maxid
 * number of tuples of each segment == @param maxid
 * each segment have an unique key value, while there exists duplicate tuples between segments
*/
int create_relation_fk(relation_t *relation, int64_t num_tuples, const int64_t maxid)
{  
    int32_t i, iters;
    int64_t remainder;
    relation_t tmp;
    check_seed();
    relation->num_tuples = num_tuples;
    relation->tuples = (tuple_t*)MALLOC(relation->num_tuples * sizeof(tuple_t));
      
    if (!relation->tuples) { 
        perror("out of memory");
        return -1; 
    }
    /* alternative generation method */
    iters = num_tuples / maxid;
    for(i = 0; i < iters; i++){
        tmp.num_tuples = maxid;
        tmp.tuples = relation->tuples + maxid * i;
        random_unique_gen(&tmp);
    }

    /* if num_tuples is not an exact multiple of maxid */
    remainder = num_tuples % maxid;
    if(remainder > 0) {
        tmp.num_tuples = remainder;
        tmp.tuples = relation->tuples + maxid * iters;
        random_unique_gen(&tmp);
    }

#ifdef PERSIST_RELATIONS
    write_relation(relation, "S.tbl");
#endif

    return 0;
}

/** allocate space for relation, 
*/
int malloc_relation(relation_t *relation, int64_t num_tuples)
{
    relation->num_tuples = num_tuples;
    relation->tuples = (tuple_t*)MALLOC(relation->num_tuples * sizeof(tuple_t));
    if (!relation->tuples) { 
        perror("out of memory");
        return -1; 
    }
    return 0;
}

int create_relation_nonunique(relation_t *relation, int64_t num_tuples,
                              const int64_t maxid) 
{
    check_seed();

    relation->num_tuples = num_tuples;
    relation->tuples = (tuple_t*)MALLOC(relation->num_tuples * sizeof(tuple_t));
    
    if (!relation->tuples) { 
        perror("out of memory");
        return -1; 
    }

    random_gen(relation, maxid);

    return 0;
}

void 
delete_relation(relation_t * rel) 
{
    /* clean up */
    FREE(rel->tuples, rel->num_tuples * sizeof(tuple_t));
}

