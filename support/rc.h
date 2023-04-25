#ifndef __RC_H__
#define __RC_H__

#define FOREACH_RC(RC)                  \
        RC(HT_OK)                       \
        RC(HT_MEM_ALLOC_FAILED)         \
        RC(HT_DPU_ALLOC_FAILED)         \
        RC(HT_DPU_LAUNCH_FAILED)        \
        RC(HT_DPU_LOAD_FAILED)          \
        RC(HT_DPU_FREE_FAILED)          \
        RC(HT_COPY_TO_ERROR)            \
        RC(HT_COPY_FROM_ERROR)          \
        RC(HT_BUFFER_FULL)              \
        RC(HT_CREATE_TWICE)             \
        RC(HT_INVALID_ARGUMENT)         \
        RC(HT_SIZE_INVALID)               \
        RC(HT_ITEM_EXISTS)              \
        RC(HT_ITEM_NOT_EXISTS)          \
        RC(HT_TABLE_FULL)               \
        RC(HT_OPERATION_UNSUPPORTED)    \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

typedef enum RC {
    FOREACH_RC(GENERATE_ENUM)
} RC;

static const char *RC_STR[] = {
    FOREACH_RC(GENERATE_STRING)
};

#endif