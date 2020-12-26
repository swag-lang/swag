public namespace time;

alias TimerHandle = const *void

struct Timer
{
    userLambda: func(Timer)
    handle:     TimerHandle
}