public struct Log
{
    lock:   sync.Mutex
    buf:    String
}

public impl Log
{
    // Create a new log
    func create()->Log
    {
        var result: retval
        result.init()
        return result
    }

    // Initialize logger
    func init(using self)
    {
    }

    func print()
    {
    }
}