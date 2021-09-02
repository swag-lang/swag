private var g_Argv: [128] string
private var g_Argc = 0'u32

func init(x: string)
{
    cmdLine := cast(const *u8) @dataof(x)

    g_Argc = 0
    while dref cmdLine
    {
        while dref cmdLine == " "'u8 cmdLine += 1
        if dref cmdLine == 0 break

        if dref cmdLine == "\""'u8
        {
            cmdLine += 1
            start := cmdLine
            while dref cmdLine and dref cmdLine != "\""'u8 cmdLine += 1
            g_Argv[g_Argc] = @mkstring(start, cast(uint) (cmdLine - start))
            if dref cmdLine cmdLine += 1
        }
        else
        {
            start := cmdLine
            while dref cmdLine and dref cmdLine != " "'u8 cmdLine += 1
            g_Argv[g_Argc] = @mkstring(start, cast(uint) (cmdLine - start))
        }

        g_Argc += 1
        while dref cmdLine and dref cmdLine == " "'u8 cmdLine += 1
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