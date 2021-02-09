#global if WINDOWS
#global public
#global namespace error
using win32

// Fill the context error message
#[swag.haserror]
protected func setErrorWin32(errorMessageID: u32)
{
    cxt := @getcontext()
    cxt.errorMsgLen = 0
    if errorMessageID == 0 return

    messageBuffer := &cxt.errorMsg[0]
    lang := MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT)
    cxt.errorMsgLen = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, null, errorMessageID, lang, cast(LPCSTR) messageBuffer, cast(DWORD) swag.MaxLenErrorMsg, null);
    if !cxt.errorMsgLen
        return

    // Trim right, remove trailing dots, remove trailing non printable characters
    for idx := cxt.errorMsgLen - 1; idx >= 0; idx -= 1
    {
        if messageBuffer[idx] > 32 && messageBuffer[idx] != "."'u8
            break
        cxt.errorMsgLen -= 1
    }
}