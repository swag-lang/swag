#include <stdint.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include <tchar.h>

typedef void(__cdecl* _PVFV)();

/*#pragma section(".CRT$XCA", read, write)
#pragma data_seg(".CRT$XCA") // start of ctor section
_PVFV __xc_a[] = {0};

#pragma section(".CRT$XCZ", read, write)
#pragma data_seg(".CRT$XCZ") // end of ctor section
_PVFV __xc_z[] = {0};

#pragma data_seg()
#pragma comment(linker, "/merge:.CRT=.rdata")*/

//int     __argc;
//TCHAR** __targv;

extern TCHAR*       _argv[];
static _PVFV*       atexitlist         = 0;
static unsigned int max_atexit_entries = 0;
static unsigned int cur_atexit_entries = 0;

#define _MAX_CMD_LINE_ARGS 32
TCHAR*        _argv[_MAX_CMD_LINE_ARGS + 1];
static TCHAR* _rawCmd = 0;

int _init_args()
{
    _argv[0] = 0;

    TCHAR* sysCmd   = GetCommandLine();
    int    szSysCmd = lstrlen(sysCmd);

    // copy the system command line
    TCHAR* cmd = (TCHAR*) HeapAlloc(GetProcessHeap(), 0, sizeof(TCHAR) * (szSysCmd + 1));
    _rawCmd    = cmd;
    if (!cmd)
        return 0;
    lstrcpy(cmd, sysCmd);

    // Handle a quoted filename
    if (*cmd == _T('"'))
    {
        cmd++;
        _argv[0] = cmd; // argv[0] = exe name

        while (*cmd && *cmd != _T('"'))
            cmd++;

        if (*cmd)
            *cmd++ = 0;
        else
            return 0; // no end quote!
    }
    else
    {
        _argv[0] = cmd; // argv[0] = exe name

        while (*cmd && !_istspace(*cmd))
            cmd++;

        if (*cmd)
            *cmd++ = 0;
    }

    int argc = 1;
    for (;;)
    {
        while (*cmd && _istspace(*cmd)) // Skip over any whitespace
            cmd++;

        if (*cmd == 0) // End of command line?
            return argc;

        if (*cmd == _T('"')) // Argument starting with a quote???
        {
            cmd++;

            _argv[argc++] = cmd;
            _argv[argc]   = 0;

            while (*cmd && *cmd != _T('"'))
                cmd++;

            if (*cmd == 0)
                return argc;

            if (*cmd)
                *cmd++ = 0;
        }
        else
        {
            _argv[argc++] = cmd;
            _argv[argc]   = 0;

            while (*cmd && !_istspace(*cmd))
                cmd++;

            if (*cmd == 0)
                return argc;

            if (*cmd)
                *cmd++ = 0;
        }

        if (argc >= _MAX_CMD_LINE_ARGS)
            return argc;
    }
}

void _term_args()
{
    if (_rawCmd)
        HeapFree(GetProcessHeap(), 0, _rawCmd);
}

void _initterm(_PVFV* pfbegin, _PVFV* pfend)
{
    // walk the table of function pointers from the bottom up, until
    // the end is encountered.  Do not skip the first entry.  The initial
    // value of pfbegin points to the first valid entry.  Do not try to
    // execute what pfend points to.  Only entries before pfend are valid.
    while (pfbegin < pfend)
    {
        if (*pfbegin != 0)
            (**pfbegin)();
        ++pfbegin;
    }
}

void _init_atexit()
{
    max_atexit_entries = 128;
    atexitlist         = (_PVFV*) calloc(max_atexit_entries, sizeof(_PVFV*));
}

int atexit(_PVFV func)
{
    if (cur_atexit_entries < max_atexit_entries)
    {
        atexitlist[cur_atexit_entries++] = func;
        return 0;
    }

    return -1;
}

void _doexit()
{
    if (cur_atexit_entries)
    {
        // Use ptr math to find the end of the array
        _initterm(atexitlist, atexitlist + cur_atexit_entries);
    }
}

/*void exit(int code)
{
    _doexit();
    ExitProcess(code);
}*/

extern "C" void mainCRTStartup()
{
    __argc  = _init_args();
    __targv = _argv;
    //_init_file();

    TCHAR* cmd = GetCommandLine();

    // Skip program name
    if (*cmd == _T('"'))
    {
        while (*cmd && *cmd != _T('"'))
            cmd++;
        if (*cmd == _T('"'))
            cmd++;
    }
    else
    {
        while (*cmd > _T(' '))
            cmd++;
    }

    // Skip any white space
    while (*cmd && *cmd <= _T(' '))
        cmd++;

    STARTUPINFO si;
    si.dwFlags = 0;
    GetStartupInfo(&si);

    _init_atexit();
    //_initterm(__xc_a, __xc_z); // call C++ constructors

    extern int main(int, char* argv[]);
    int        ret = main(__argc, __targv); // _tWinMain(GetModuleHandle(0), 0, cmd, si.dwFlags & STARTF_USESHOWWINDOW ? si.wShowWindow : SW_SHOWDEFAULT);

    _doexit();
    _term_args();
    ExitProcess(ret);
}

extern "C" void __chkstk()
{
}

extern "C" int _fltused = 0;
BOOL WINAPI    DllMain(HANDLE, DWORD, LPVOID);

extern "C" BOOL _DllMainCRTStartup(HANDLE hInst, DWORD reason, LPVOID imp)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        _init_atexit();
        //_init_file();
        //_initterm(__xc_a, __xc_z);
    }

    //BOOL ret = DllMain(hInst, reason, imp);

    if (reason == DLL_PROCESS_DETACH)
    {
        _doexit();
    }

    return TRUE;
}
