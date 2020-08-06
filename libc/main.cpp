#include <stdint.h>
#include "libc.h"

static char* _argv[MAX_COMMAND_ARGUMENTS + 1];
static char* _rawCmd = 0;

static int initArgs(const char* sysCmd)
{
    _argv[0] = 0;

    int lenCmd = (int) strlen(sysCmd);

    char* cmd = (char*) malloc(lenCmd + 1);
    _rawCmd   = cmd;
    if (!cmd)
        return 0;

    memcpy(cmd, sysCmd, lenCmd + 1);

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

        if (argc >= MAX_COMMAND_ARGUMENTS)
            return argc;
    }
}

static void termArgs()
{
    if (_rawCmd)
        free(_rawCmd);
}

void __main(const char* cmdLine)
{
    int        argc = initArgs(cmdLine);
    extern int main(int, char* argv[]);
    int        ret = main(argc, _argv);
    termArgs();
    exit(ret);
}
