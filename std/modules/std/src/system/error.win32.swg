#if std.WINDOWS;
public namespace std.error;
using win32

// Fill the context error message
func setLastErrorString()
{
    cxt := @getcontext()
    cxt.errorMsgLen = 0

    errorMessageID := GetLastError()
    if errorMessageID == 0 return

    messageBuffer := &cxt.errorMsg[0]
    lang := MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT)
    cxt.errorMsgLen = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, null, errorMessageID, lang, cast(LPCSTR) messageBuffer, cast(DWORD) swag.MaxLenErrorMsg, null);
}