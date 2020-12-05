#if #os == "windows";
public namespace std.process;
using win32

// Starts a new process with arguments
func start(info: StartInfo) -> {proc: Process, err: ErrorId}
{
    debug.assert(info.fileName != null, "missing process filename")

    var result: retval

    var si: STARTUPINFOA
    var pi: PROCESS_INFORMATION
    si.cb = @sizeof(si)

    // Construct the command line
    var builder: strconv.StringBuilder
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
    {
        error.setLastErrorId()
        result.err = error.last()
        return result
    }

    result.proc.handle = pi.hProcess
    return result
}