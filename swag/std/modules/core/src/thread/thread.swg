#global public
#global namespace thread

alias ThreadHandle = const *void

enum ThreadPriority
{
    Lowest
    BelowNormal
    Normal
    AboveNormal
    Highest
}

struct Thread
{
    userLambda: func(Thread)

    context:    swag.Context
    userParam:  *void

    priority =  ThreadPriority.Normal
    handle:     ThreadHandle
    id:         u32
}

impl Thread
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