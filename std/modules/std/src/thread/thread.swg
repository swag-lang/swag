public namespace std.thread;
using std;

alias ThreadHandle = const *void

struct Thread
{
    handle:     ThreadHandle
    id:         u32
    userLambda: func(Thread)
    userParam:  *void
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
        return userLambda != null
    }
}