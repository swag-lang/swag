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

impl IEvent for BaseEvent {}
impl IEvent for MouseEvent {}

#[Swag.Safety("", false)]
//#[Swag.PrintBc]
func toto(x: IEvent)
{
    var g = 0
    switch x
    {
    case MouseEvent: g = 1
    }

    let ptr = cast(*BaseEvent) x
    @assert(ptr.x == 66)
}

#[Swag.Safety("", false)]
//#[Swag.PrintBc]
#test
{
    var evt: MouseEvent
    //@breakpoint()
    toto(cast(IEvent) evt)
}