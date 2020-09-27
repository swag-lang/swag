#include "swag_runtime.h"
#include "libc/libc.h"

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void swag_runtime_assert_msg(ConcreteCompilerSourceLocation* location, const void* message)
{
    // During tests, and if not in devmode, then just raise an error
    SwagContext* context = (SwagContext*) swag_runtime_tlsGetValue(g_SwagProcessInfos.contextTlsId);
    if ((context->flags & (SwagU64) ContextFlags::ByteCode) ||
        ((context->flags & (SwagU64) ContextFlags::Test) && !(context->flags & (SwagU64) ContextFlags::DevMode)))
    {
        if (!message)
            message = "assertion failed";
        swag_runtime_error(location, message, (SwagU32) strlen((const char*) message));
        return;
    }

    static char buf[1024];
    static char bufline[64];

    const char* msg = (const char*) message;
    buf[0]          = 0;
    if (msg && msg[0])
    { 
        strcat(buf, msg);
        strcat(buf, "\n\n");
    }
    else
    {
        strcat(buf, "assertion failed\n\n");
    }

    strcat(buf, (const char*) location->fileName.buffer);
    strcat(buf, ", line ");
    swag_runtime_itoa(bufline, location->lineStart);
    strcat(buf, bufline);

#ifdef _WIN32
    // No need to convert to unicode as this assert does not allow user messages
    auto result = MessageBoxA(0, buf, "Intrinsic assertion failed", MB_ICONHAND | MB_CANCELTRYCONTINUE);
    switch (result)
    {
    case IDCANCEL:
        exit(-666);
        break;
    case IDTRYAGAIN:
        DebugBreak();
        break;
    case IDCONTINUE:
        break;
    }
#else
    exit(-666);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
EXTERN_C void swag_runtime_assert(bool expr, const void* file, SwagU64 colline, const void* message)
{
    if (expr)
        return;
    ConcreteCompilerSourceLocation loc;
    loc.lineStart = loc.lineEnd = (SwagU32)(colline & 0xFFFFFFFF);
    loc.colStart = loc.colEnd = (SwagU32)(colline >> 32);
    loc.fileName.buffer       = (void*) file;
    loc.fileName.count        = strlen((const char*) file);
    swag_runtime_assert_msg(&loc, message);
}

/////////////////////////////////////////////////////////////////////////////////////////////
void swag_runtime_error(ConcreteCompilerSourceLocation* location, const void* message, SwagU32 size)
{
    SwagContext* context = (SwagContext*) swag_runtime_tlsGetValue(g_SwagProcessInfos.contextTlsId);
    if (context->flags & (SwagU64) ContextFlags::ByteCode)
    {
#ifdef _WIN32
        // Raise an exception that will be catched by the runner.
        // The runner is in charge of displaying the user error message and location
        static void* params[3];
        params[0] = (void*) location;
        params[1] = (void*) message;
        params[2] = (void*) (SwagSizeT) size;
        RaiseException(666, 0, 3, &params);
#endif
    }

    swag_runtime_print("error: ");
    swag_runtime_print_n(location->fileName.buffer, (SwagU32) location->fileName.count);
    swag_runtime_print(":");
    swag_runtime_print_i64(location->lineStart + 1);
    swag_runtime_print(": ");
    swag_runtime_print_n(message, size);
    swag_runtime_print("\n");
    exit(-666);
}
