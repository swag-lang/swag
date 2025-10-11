#global testerror "Saf0018"

interface IEvent
{
}
struct Mouse {}
impl IEvent for Mouse
{
}

//#[Swag.PrintBc]
#[Swag.Safety("", true), Swag.Sanity(false)]
#test
{
    var x:  IEvent
    let ptr = cast(*Mouse) x
    @print(ptr)
}
