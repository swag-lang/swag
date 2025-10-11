var g_Argv: [128] string
var g_Argc  = 0'u32

func init(x: string)
{
    var cmdLine = cast(const [*] u8) @dataof(x)

    g_Argc = 0
    while dref cmdLine
    {
        while dref cmdLine == ' ''u8 do
            cmdLine += 1
        if dref cmdLine == 0 do
            break

        if dref cmdLine == '"''u8
        {
            cmdLine += 1
            let start = cmdLine
            while dref cmdLine and dref cmdLine != '"''u8 do
                cmdLine += 1
            g_Argv[g_Argc] = @mkstring(start, cast(u64) (cmdLine - start))
            if dref cmdLine do
                cmdLine += 1
        }
        else
        {
            let start = cmdLine
            while dref cmdLine and dref cmdLine != ' ''u8 do
                cmdLine += 1
            g_Argv[g_Argc] = @mkstring(start, cast(u64) (cmdLine - start))
        }

        g_Argc += 1
        while dref cmdLine and dref cmdLine == ' ''u8 do
            cmdLine += 1
    }
}

#test
{
    init("a b cc")
    @assert(g_Argc == 3)
    @assert(g_Argv[0] == "a")
    @assert(g_Argv[1] == "b")
    @assert(g_Argv[2] == "cc")

    init(#""this is" cc"#)
    @assert(g_Argc == 2)
    @assert(g_Argv[0] == "this is")
}
