struct BaseEvent
{
    x    = 66
}

struct MouseEvent
{
    using base: BaseEvent
}

interface IEvent
{
}

impl IEvent for BaseEvent
{
}
impl IEvent for MouseEvent
{
}

func toto(x: IEvent)
{
    var g = 0
    switch x
    {
    case MouseEvent:
        let ptr = cast(*MouseEvent) x
        g = ptr.x
    }

    @assert(g == 66)
}

#test
{
    var evt: MouseEvent
    //@breakpoint()
    toto(cast(IEvent) evt)
}
