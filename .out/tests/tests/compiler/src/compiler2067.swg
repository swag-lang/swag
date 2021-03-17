private var g_Argv: [128] string
private var g_Argc = 0'u32

func init(x: string)
{
    cmdLine := cast(const *u8) @dataof(x)

    g_Argc = 0
    while :cmdLine
    {
        while :cmdLine == " "'u8 cmdLine += 1
        if :cmdLine == 0 break

        if :cmdLine == "\""'u8
        {
            cmdLine += 1
            start := cmdLine
            while :cmdLine && :cmdLine != "\""'u8 cmdLine += 1
            g_Argv[g_Argc] = @mkstring(start, cast(uint) (cmdLine - start))
            if :cmdLine cmdLine += 1
        }
        else
        {
            start := cmdLine
            while :cmdLine && :cmdLine != " "'u8 cmdLine += 1
            g_Argv[g_Argc] = @mkstring(start, cast(uint) (cmdLine - start))
        }

        g_Argc += 1
        while :cmdLine && :cmdLine == " "'u8 cmdLine += 1
    }
}

#test
{
    init("a b cc")
    @assert(g_Argc == 3)
    @assert(g_Argv[0] == "a")
    @assert(g_Argv[1] == "b")
    @assert(g_Argv[2] == "cc")

    init(@""this is" cc"@)
    @assert(g_Argc == 2)
    @assert(g_Argv[0] == "this is")
}