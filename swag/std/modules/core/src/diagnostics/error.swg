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
}