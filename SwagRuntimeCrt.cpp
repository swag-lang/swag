#include <stdint.h>
#include "SwagRuntime.h"
#ifdef _WIN32
#include <windows.h>
#endif

static char* _argv[SWAG_MAX_COMMAND_ARGUMENTS + 1];
static char* _rawCmd = 0;
extern char* _argv[];

static int initArgs(const char* sysCmd)
{
    _argv[0] = 0;

    int szSysCmd = (int) strlen(sysCmd);

    char* cmd = (char*) malloc(szSysCmd + 1);
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
        free(_rawCmd);
}

static void __main(const char* cmdLine)
{
    int        argc = initArgs(cmdLine);
    extern int main(int, char* argv[]);
    int        ret = main(argc, _argv);
    termArgs();
    exit(ret);
}

#ifdef _WIN32
extern "C" int _fltused = 0;

extern "C" void mainCRTStartup()
{
    __main(::GetCommandLine());
}

extern "C" BOOL _DllMainCRTStartup(HANDLE hInst, DWORD reason, LPVOID imp)
{
    return TRUE;
}

extern "C" void __chkstk()
{
}

#endif // _WIN32
