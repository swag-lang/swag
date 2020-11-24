public namespace std.thread;
using std;

alias MutexHandle = const *void

struct Mutex
{
    handle: MutexHandle
}

impl Mutex
{
    // Returns true if the mutex is valid
    func isValid(using self)->bool
    {
        return handle != null
    }
}