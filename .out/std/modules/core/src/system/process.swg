#global public
#global namespace Env

alias ProcessHandle = const *void

struct StartInfo
{
    fileName:   string
    arguments:  string
}

struct Process
{
    handle: ProcessHandle
}

// Starts a new process with arguments
func startProcess(fileName: string, arguments: string = null)->Process throw
{
    var info: StartInfo
    info.fileName = fileName
    info.arguments = arguments
    return try startProcess(info)
}