public namespace std.error;

// Returns the error string associated with the current thread
func last()->string
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