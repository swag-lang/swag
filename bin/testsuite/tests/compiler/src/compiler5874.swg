// Get the program command line arguments as a [..] string
func getArgs()->const [..] string => @args

// Get a given argument, or null
func getArg(idx: u32)->#null string
{
    let args = @args
    if idx >= @countof(args):
        return null
    return args[idx]
}
