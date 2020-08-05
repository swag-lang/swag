#include <stdint.h>
#include "SwagRuntime.h"
#ifdef _WIN32
#include <windows.h>
#endif

static char*   _argv[SWAG_MAX_COMMAND_ARGUMENTS + 1];
static char*   _rawCmd = 0;
extern char*   _argv[];
extern "C" int _fltused = 0;

static int _init_args()
{
    _argv[0] = 0;

    char* sysCmd   = ::GetCommandLine();
    int   szSysCmd = (int) strlen(sysCmd);

    // Copy the system command line
    TCHAR* cmd = (TCHAR*) HeapAlloc(GetProcessHeap(), 0, sizeof(TCHAR) * (szSysCmd + 1));
    _rawCmd    = cmd;
    if (!cmd)
        return 0;
    lstrcpy(cmd, sysCmd);

    // Handle a quoted filename
    if (*cmd == '"')
    {
        cmd++;
        _argv[0] = cmd; // argv[0] = exe name

        while (*cmd && *cmd != '"')
            cmd++;

        if (*cmd)
            *cmd++ = 0;
        else
            return 0; // no end quote!
    }
    else
    {
        _argv[0] = cmd; // argv[0] = exe name

        while (*cmd && !isspace(*cmd))
            cmd++;

        if (*cmd)
            *cmd++ = 0;
    }

    int argc = 1;
    for (;;)
    {
        while (*cmd && isspace(*cmd)) // Skip over any whitespace
            cmd++;

        if (*cmd == 0) // End of command line?
            return argc;

        if (*cmd == '"') // Argument starting with a quote???
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

static void _term_args()
{
    if (_rawCmd)
        HeapFree(GetProcessHeap(), 0, _rawCmd);
}

extern "C" void mainCRTStartup()
{
    __argc = _init_args();
    __argv = _argv;

    TCHAR* cmd = GetCommandLine();

    // Skip program name
    if (*cmd == '"')
    {
        while (*cmd && *cmd != '"')
            cmd++;
        if (*cmd == '"')
            cmd++;
    }
    else
    {
        while (*cmd > ' ')
            cmd++;
    }

    // Skip any white space
    while (*cmd && *cmd <= ' ')
        cmd++;

    extern int main(int, char* argv[]);
    int        ret = main(__argc, __argv);

    _term_args();
    ::ExitProcess(ret);
}

extern "C" BOOL _DllMainCRTStartup(HANDLE hInst, DWORD reason, LPVOID imp)
{
    return TRUE;
}

extern "C" void __chkstk()
{
}