#pragma once
#include "Os.h"
union Register;
struct ByteCode;

struct Context
{
    ByteCode* allocator;
};

typedef void (*swag_allocator_t)(Register*);
typedef struct swag_context_t
{
    swag_allocator_t allocator;
} swag_context_t;

typedef struct swag_slice_t
{
    void*    addr;
    uint64_t count;
} swag_slice_t;

typedef struct swag_process_infos_t
{
    swag_slice_t    arguments;
    swag_tls_id_t   contextTlsId;
    swag_context_t* defaultContext;
} swag_process_infos_t;

extern swag_tls_id_t        g_tlsContextIdByteCode;
extern swag_tls_id_t        g_tlsContextIdBackend;
extern Context              g_defaultContextByteCode;
extern swag_context_t       g_defaultContextBackend;
extern swag_process_infos_t g_processInfos;

extern void initDefaultContext();