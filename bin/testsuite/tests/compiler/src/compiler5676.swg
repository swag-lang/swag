#global testerror "Err0768"
var a = cast<toto>(s32) 0

#global skip
struct BaseEvent 
{
    x = 66
}

struct EventMouse
{
    using base: BaseEvent
}

#[Swag.Safety("", false)]
#[Swag.PrintBc]
func toto(x: any)
{
    let ptr = cast(*BaseEvent) dref cast(**void) @dataof(x)
    @print(cast(u64) ptr, " - ")
    @print(cast(u64) ptr.x)
    @print(" ")    

    /*@print(cast(u64) cast(*EventMouse) x)
    @print(" ")    
    @print(cast(u64) @dataof(x))
    @print(" ")
    let ptr = cast(*BaseEvent) @dataof(x)
    @print(cast(u64) ptr)*/

    @print(" ")
    let ptr1 = cast(*EventMouse) x
    @print(cast(u64) ptr1, " - ")
    @print(cast(u64) ptr1.x)
}

#[Swag.Safety("", false)]
#[Swag.PrintBc]
#test
{
    var evt: EventMouse
    //@breakpoint()
    toto(&evt)
}