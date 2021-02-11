#global public
#global namespace process

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
func start(fileName: string, arguments: string = null)->Process
{
    var info: StartInfo
    info.fileName = fileName
    info.arguments = arguments
    return try start(info)
}