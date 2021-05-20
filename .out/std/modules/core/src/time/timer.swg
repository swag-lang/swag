#global public
#global namespace Time

alias TimerHandle = const *void

struct Timer
{
    userLambda: func(Timer)
    handle:     TimerHandle
}