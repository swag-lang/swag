#global public
#global namespace sync
alias EventHandle = const *void

#[swag.nocopy]
struct Event
{
    handle: MutexHandle
}

impl Event
{
    // Returns true if the event is valid
    func isValid(using self)->bool
    {
        return handle != null
    }
}