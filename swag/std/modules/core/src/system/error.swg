#global public

alias ErrorId = string

namespace error
{
    // Returns the error string associated with the current thread
    // The returned string is valid until a new error is raised (in that
    // case the string will be changed to the new error)
    func last()->ErrorId
    {
        cxt := @getcontext()
        if cxt.errorMsgLen == 0 return null
        return cast(string) @mkslice(&cxt.errorMsg[0], cast(uint) cxt.errorMsgLen)
    }

    // Clear the last error message
    func clear()
    {
        cxt := @getcontext()
        cxt.errorMsgLen = 0
    }

    func fatal(msg: string, args: ...)
    {
        console.print(msg, args)
        process.exit(cast(u32) -1)
    }
}