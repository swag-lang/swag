#global if WINDOWS
#global public
#global namespace process
using win32, strconv

// Exit the current process
func exit(exitCode: u32) => ExitProcess(exitCode)

// Starts a new process with arguments
func start(info: StartInfo)->Process throw
{
    debug.assert(info.fileName != null, "missing process filename")

    var result: retval

    var si: STARTUPINFOA
    var pi: PROCESS_INFORMATION
    si.cb = @sizeof(si)

    // Construct the command line
    var builder: StringBuilder
    builder.appendString(info.fileName)
    if info.arguments
    {
        builder.appendString(" ")
        builder.appendString(info.arguments)
    }

    builder.zeroTerminated()
    str := builder.moveToString()

    // Launch
    if !CreateProcessA(null, @dataof(str), null, null, FALSE, 0, null, null, &si, &pi)
        try win32.setError(GetLastError())

    result.handle = pi.hProcess
    return result
}