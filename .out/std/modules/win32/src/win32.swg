#global export

// Fill the context error message
func setError(errorMessageID: u32) throw
{
    if errorMessageID == 0 return

    var errorMsg: [swag.MaxLenErrorMsg] u8
    var len: u32

    lang := cast(DWORD) MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT)
    len = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, null, errorMessageID, lang, cast(LPCSTR) &errorMsg[0], cast(DWORD) swag.MaxLenErrorMsg, null);
    if !len return

    // Trim right, remove trailing dots, remove trailing non printable characters
    for idx := len - 1; idx >= 0; idx -= 1
    {
        if errorMsg[idx] > 32 && errorMsg[idx] != "."'u8
            break
        len -= 1
    }

    throw @mkstring(&errorMsg[0], len)
}