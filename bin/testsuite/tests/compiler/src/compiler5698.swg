struct Evt
{
    x:   s32
}

struct Evt1
{
    using base: Evt
}

struct Evt2
{
    using base: Evt
}

struct Evt3
{
    using base: *Evt2
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
    @assert(@is(Evt1, @kindof(v)) == false)
    @assert(@is(Evt2, @kindof(v)) == true)
    @assert(@is(Evt3, @kindof(v)) == true)
}

#test
{
    var b: Evt3
    toto(b)
}
