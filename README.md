# pim-hashjoin
Partitioning and Non-partitioning Hash Join Implementation

# File layout
Directory host have the code run on host
Directory dpu has the code run on dpu
Directory support has the common structs and attributes shared by host and dpu
Directory mram_alloc is the buddy allocator for mram
host/affinity.h configures host attributes
host/app.c is the main code running on host
host/generator.* generates relation microbenchmark

# Hash method
pure modulo method
There are two hash function attributes
One is hash start bit, verify which bits to hash
One is hash length bit, verify how many hits to hash

+----------32 bits----------+
|          key              |
+-------|-------------------+
|    hash start bit 1       |
|       |length 1  |        |
+------------------|--------+
|          hash start bit 2 |
|                  |length 2|
+---------------------------+
