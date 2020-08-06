#ifdef _WIN32
#include <stdint.h>
#include "../libc.h"

EXTERN_C
{
    int _fltused = 0;

    void __chkstk()
    {
    }

    void mainCRTStartup()
    {
        __main(GetCommandLineA());
    }

    int _DllMainCRTStartup(void*, int, void*)
    {
        return 1;
    }
}

#endif // _WIN32
