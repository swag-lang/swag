#global public
#global namespace Env

alias ProcessHandle = const *void

struct StartInfo
{
    fileName:      string
    arguments:     string
}

struct Process
{
    handle: ProcessHandle
}

// Starts a new process with arguments
#[Swag.Overload]
func startProcess(fileName: string, arguments: string = null)->Process throw
{
    var info: StartInfo
    info.fileName  = fileName
    info.arguments = arguments
    return try startProcess(info)
}

// Starts a new process with arguments, and wait for it to be done
func doSyncProcess(fileName: string, arguments: string = null) throw
{
    var h = try startProcess(fileName, arguments)
    h.waitForExit()
}
