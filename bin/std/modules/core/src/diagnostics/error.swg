#global public

namespace Console
{
    // Fatal error
    // Exit the process with code -1
    func fatal(msg: string, args: ...)
    {
        Console.printf(msg, args)
        Console.println()
        Env.exit(cast(u32) #bit -1)
    }
}
