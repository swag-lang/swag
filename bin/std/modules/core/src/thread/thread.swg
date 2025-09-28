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
    userLambda:     func||(Thread)

    context:        Swag.Context
    userParam:      *void

    priority        = ThreadPriority.Normal
    handle:         ThreadHandle
    id:             u32
    requestEnd      = false
}

impl Thread
{
    mtd opDrop()
    {
        Debug.assert(!.isValid(), "thread is not terminated")
    }

    // Returns true if the thread is valid
    mtd const isValid()->bool
    {
        return .handle != null
    }

    // Force the thread to safely exist
    // User code needs to check for 'requestEnd'
    mtd safeForceEnd()
    {
        .requestEnd = true
    }
}
