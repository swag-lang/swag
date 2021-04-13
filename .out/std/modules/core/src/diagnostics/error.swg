#global public

namespace error
{
    // Fatal error
    // Exit the process with code -1
    func fatal(msg: string, args: ...)
    {
        console.printf(msg, args)
        console.println()
        process.exit(bitcast(u32) -1)
    }
}