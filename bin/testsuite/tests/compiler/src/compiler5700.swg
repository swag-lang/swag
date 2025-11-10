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
    let ptr = cast(*Evt2) @as(Evt2, @kindof(v), @dataof(v))
    @assert(ptr.x == 4)

    let ptr1 = cast(*Evt1) @as(Evt1, @kindof(v), @dataof(v))
    @assert(ptr1 == null)
}

#test
{
    var b: Evt3
    toto(b)
}
