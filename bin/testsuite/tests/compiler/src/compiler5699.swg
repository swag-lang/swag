struct Evt
{
    x:   s32 = 4
}

struct Evt1
{
    aaaa:           s32
    using base:     Evt
}

struct Evt2
{
    aaa:            s32
    using base:     Evt
}

struct Evt3
{
    aa:             s32
    using base:     Evt2
}

interface IEvent
{
}
impl IEvent for Evt
{
}
impl IEvent for Evt1
{
}
impl IEvent for Evt2
{
}
impl IEvent for Evt3
{
}

func toto(v: IEvent)
{
    //@assert(@is(Evt1, @kindof(v)) == false)
    //@assert(@is(Evt2, @kindof(v)) == true)
    //@assert(@is(Evt3, @kindof(v)) == true)
    let ptr = cast(*Evt3) @as(Evt3, @kindof(v), @dataof(v))
    @assert(ptr.x == 4)
}

#test
{
    var b: Evt3
    toto(b)
}
