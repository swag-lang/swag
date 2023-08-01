#global public
#global namespace Time

typealias TimerHandle = const *void

struct Timer
{
    userLambda: func(Timer)
    handle:     TimerHandle
    context:    Swag.Context
}