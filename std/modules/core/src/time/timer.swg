public namespace time;

alias TimerHandle = const *void

struct Timer
{
    // Must be first
    userLambda: func(Timer)

    handle: TimerHandle
}