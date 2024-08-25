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
    case MouseEvent: 
        let ptr = cast(*MouseEvent) x
        @assert(ptr.x == 66)
        g = 1
    }

    @assert(g == 1)
}

#[Swag.Safety("", false)]
//#[Swag.PrintBc]
#test
{
    var evt: MouseEvent
    //@breakpoint()
    toto(cast(IEvent) evt)
}