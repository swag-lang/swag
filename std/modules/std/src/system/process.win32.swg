#if #os == "windows";
public namespace std.process;
using win32

// Starts a new process with arguments
func start(name: string, arguments: string = null) -> {proc: Process, err: ErrorId}
{
    var result: retval

    var si: STARTUPINFOA
    var pi: PROCESS_INFORMATION
    si.cb = @sizeof(si)

    // Construct the command line
    var builder: strconv.StringBuilder
    builder.appendString(name)
    if arguments
    {
        builder.appendString(" ")
        builder.appendString(arguments)
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