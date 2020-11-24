#if std.WINDOWS;
public namespace std.error;
using win32

// Fill the context error message
func setLastErrorId()
{
    // Should be called first, before @getcontext(), which can reset
    // the last error id
    errorMessageID := GetLastError()

    cxt := @getcontext()
    cxt.errorMsgLen = 0
    if errorMessageID == 0 return

    messageBuffer := &cxt.errorMsg[0]
    lang := MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT)
    cxt.errorMsgLen = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, null, errorMessageID, lang, cast(LPCSTR) messageBuffer, cast(DWORD) swag.MaxLenErrorMsg, null);
}