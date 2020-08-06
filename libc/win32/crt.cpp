#ifdef _WIN32
#include <stdint.h>
#include "../libc.h"

EXTERN_C int _fltused = 0;

EXTERN_C void mainCRTStartup()
{
    const char* GetCommandLineA();
    __main(GetCommandLineA());
}

EXTERN_C int _DllMainCRTStartup(void*, int, void*)
{
    return 1;
}

extern "C" void __chkstk()
{
}

#endif // _WIN32
