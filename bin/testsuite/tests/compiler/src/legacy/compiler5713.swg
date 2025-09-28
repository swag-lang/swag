#global testerror "San0006"

interface IEvent {}
struct Mouse {}
impl IEvent for Mouse {}

//#[Swag.PrintBc]
#[Swag.Sanity(true)]
#test
{
    var x: IEvent
    let ptr = cast(*Mouse) x
    @print(ptr)
}