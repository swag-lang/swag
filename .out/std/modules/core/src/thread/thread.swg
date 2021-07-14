#global public
#global namespace Threading

alias ThreadHandle = const *void

enum ThreadPriority
{
    Lowest
    BelowNormal
    Normal
    AboveNormal
    Highest
}

#[Swag.NoCopy]
struct Thread
{
    userLambda: func(Thread)

    context:    Swag.Context
    userParam:  *void

    priority =  ThreadPriority.Normal
    handle:     ThreadHandle
    id:         u32
}

impl Thread
{
    func opDrop(using self)
    {
        Debug.assert(!isValid(), "thread is not terminated")
    }

    // Returns true if the thread is valid
    func isValid(using self)->bool
    {
        return handle != null
    }
}