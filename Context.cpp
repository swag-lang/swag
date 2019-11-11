#include "pch.h"
#include "Os.h"
#include "Context.h"

swag_tls_id_t  g_tlsContextIdByteCode = 0;
swag_tls_id_t  g_tlsContextIdBackend  = 0;
Context        g_defaultContextByteCode;
swag_context_t g_defaultContextBackend;
