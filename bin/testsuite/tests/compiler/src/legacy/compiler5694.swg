#global testerror "Err0202"
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

interface IEvent
{
}
impl IEvent for Evt
{
}
impl IEvent for Evt1
{
}
//impl IEvent for Evt2 {}

func toto(v: IEvent)
{
    let ptr = cast(*Evt2) v
}

#test
{
    var a: Evt
    var b: Evt1{x: 10}

    toto(b)
}
