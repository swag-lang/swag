#include <stdint.h>
#include "SwagRuntime.h"
#ifdef _WIN32
#include <windows.h>

static char*   _argv[SWAG_MAX_COMMAND_ARGUMENTS + 1];
static char*   _rawCmd = 0;
extern char*   _argv[];
extern "C" int _fltused = 0;

static int initArgs()
{
    _argv[0] = 0;

    char* sysCmd   = ::GetCommandLine();
    int   szSysCmd = (int) strlen(sysCmd);

    char* cmd = (char*) HeapAlloc(::GetProcessHeap(), 0, sizeof(TCHAR) * (szSysCmd + 1));
    _rawCmd   = cmd;
    if (!cmd)
        return 0;
    lstrcpy(cmd, sysCmd);

    // First argument is executable
    if (*cmd == '"')
    {
        cmd++;
        _argv[0] = cmd;

        while (*cmd && *cmd != '"')
            cmd++;

        if (*cmd)
            *cmd++ = 0;
        else
            return 0;
    }
    else
    {
        _argv[0] = cmd;

        while (*cmd && !isspace(*cmd))
            cmd++;

        if (*cmd)
            *cmd++ = 0;
    }

    // All other arguments
    int argc = 1;
    for (;;)
    {
        while (*cmd && isspace(*cmd))
            cmd++;

        if (*cmd == 0)
            return argc;

        if (*cmd == '"')
        {
            cmd++;

            _argv[argc++] = cmd;
            _argv[argc]   = 0;

            while (*cmd && *cmd != '"')
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

            while (*cmd && !isspace(*cmd))
                cmd++;

            if (*cmd == 0)
                return argc;

            if (*cmd)
                *cmd++ = 0;
        }

        if (argc >= SWAG_MAX_COMMAND_ARGUMENTS)
            return argc;
    }
}

static void termArgs()
{
    if (_rawCmd)
        HeapFree(GetProcessHeap(), 0, _rawCmd);
}

extern "C" void mainCRTStartup()
{
    __argc = initArgs();
    __argv = _argv;

    extern int main(int, char* argv[]);
    int        ret = main(__argc, __argv);

    termArgs();
    ::ExitProcess(ret);
}

extern "C" BOOL _DllMainCRTStartup(HANDLE hInst, DWORD reason, LPVOID imp)
{
    return TRUE;
}

extern "C" void __chkstk()
{
}

#endif // _WIN32
