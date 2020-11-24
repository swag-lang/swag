public namespace std.error;

alias ErrorId = string
const Ok: ErrorId = null

// Returns the error string associated with the current thread
func last()->ErrorId
{
    cxt := @getcontext()
    return cast(string) @mkslice(&cxt.errorMsg[0], cxt.errorMsgLen)
}

// Clear the last error message
func clear()
{
    cxt := @getcontext()
    cxt.errorMsgLen = 0
}