#ifdef _WIN32
#ifndef SWAG_RUNTIME_COMPILER

#include "../libc.h"

EXTERN_C
{
    int _fltused = 0;

    void __chkstk()
    {
    }

    /*void mainCRTStartup()
    {
        void swag_runtime_main(const char* cmdLine);
        swag_runtime_main(GetCommandLineA());
    }

    int _DllMainCRTStartup(void*, int, void*)
    {
        return 1;
    }*/
}

#endif // SWAG_RUNTIME_COMPILER
#endif // _WIN32
