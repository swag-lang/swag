#global export

// Throw a [[Swag.SystemError]]
func throwWinError(errorMessageID: u32) throw
{
    if errorMessageID == 0 
        return

    var err: Swag.SystemError
    err.errorID = errorMessageID

    const MaxLen = 128
    let errorMsg = cast(^u8) @getcontext().errorAllocator.alloc(acast MaxLen)
    let lang = cast(DWORD) MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT)
    var len = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, null, errorMessageID, lang, cast(LPCSTR) errorMsg, cast(DWORD) MaxLen, null);
    if len 
    {
        // Trim right, remove trailing dots, remove trailing non printable characters
        for var idx = len - 1; idx >= 0; idx -= 1
        {
            if errorMsg[idx] > 32 and errorMsg[idx] != `.`
                break
            len -= 1
        }

        err.message = @mkstring(errorMsg, len)
    }

    throw err
}