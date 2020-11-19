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

protected func getLastErrorString()->String
{
    var result: retval

    // Get the error message, if any.
    errorMessageID := GetLastError()
    if (errorMessageID == 0)
        return result

    var messageBuffer: LPCSTR
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    errorMessageID,
                    MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
                    cast(LPCSTR) &messageBuffer,
                    0,
                    NULL);

    //std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);
    return result
}