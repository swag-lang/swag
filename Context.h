#pragma once
struct ByteCode;

struct Context
{
    ByteCode* allocator;
};

extern uint32_t g_tlsContextId;
extern Context  g_defaultContext;

// Should match the runtime !
/*struct swag_process_infos_t
{
    swag_uint64_t   argumentsSlice[2];
    swag_tls_id_t   contextTlsId;
    swag_context_t* defaultContext;
};*/
