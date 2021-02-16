#global public

namespace error
{
    // Fatal error
    // Exit the process with code -1
    func fatal(msg: string, args: ...)
    {
        console.print(msg, args)
        process.exit(bitcast(u32) -1)
    }

    // Fatal error if an error has been raised
    // Exit the process with code -1
    func fatal()
    {
        if !@err return
        fatal(@err)
    }
}