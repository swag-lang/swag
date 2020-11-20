using std;

public alias ThreadHandle = const *void

public struct Thread
{
    handle:     ThreadHandle
    id:         u32
    userLambda: func(Thread)
    userParam:  *void
}

public impl Thread
{
    func opDrop(using self)
    {
        debug.assert(!isValid(), "thread is not terminated")
    }

    // Returns true if the thread is valid
    func isValid(using self)->bool
    {
        return handle != null
    }
}