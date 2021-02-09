#global public

namespace error
{
    func fatal(msg: string, args: ...)
    {
        console.print(msg, args)
        process.exit(bitcast(u32) -1)
    }
}