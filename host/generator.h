#ifndef GENERATOR_H
#define GENERATOR_H

#include "../support/types.h"

/**
 * Seed the random number generator before calling create_relation_xx. If not
 * called, then generator will be initialized with the time of the call which
 * produces different random numbers from run to run.
 */
void seed_generator(unsigned int seed);

/**
 * Create relation with foreign keys (i.e. duplicated keys exist). If ntuples is
 * an exact multiple of maxid, (ntuples/maxid) sub-relations with shuffled keys
 * following each other are generated.
 */
int create_relation_fk(relation_t *reln, int64_t ntuples, const int64_t maxid);

/** allocate space for relation, 
*/
int malloc_relation(relation_t *relation, int64_t num_tuples);

/**
 * Free memory allocated for only tuples.
 */
void delete_relation(relation_t * reln);

#endif /* GENERATOR_H */
