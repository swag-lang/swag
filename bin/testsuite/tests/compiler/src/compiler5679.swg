//#global testerror "Err0563"

struct BaseEvent
{
    x = 66
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
    case let ptr1: MouseEvent:
        g = ptr1.x
    }

    @assert(g == 66)
}

#test
{
    var evt: MouseEvent
    //@breakpoint()
    toto(cast(IEvent) evt)
}
