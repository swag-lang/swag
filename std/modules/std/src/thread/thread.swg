public namespace thread;

alias ThreadHandle = const *void

struct Thread
{
    handle: ThreadHandle
    lambda: func(*Thread)
    id:     u32
}
