#if std.WINDOWS;
public namespace std.environment;
using win32

const EndLine = "\r\n"

func getProcessorCount() => cast(u32) g_sysInfo.dwNumberOfProcessors

// Exit the application with a given code
func exit(exitCode: u32) => ExitProcess(exitCode)

private var g_sysInfo: SYSTEM_INFO
#init
{
    GetSystemInfo(&g_sysInfo)
}

// Fill the context error message
protected func setLastErrorString()
{
    cxt := @getcontext()
    cxt.errorMsgLen = 0

    errorMessageID := GetLastError()
    if errorMessageID == 0 return

    messageBuffer := &cxt.errorMsg[0]
    lang := MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT)
    cxt.errorMsgLen = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, null, errorMessageID, lang, cast(LPCSTR) messageBuffer, cast(DWORD) swag.MaxLenErrorMsg, null);
}