#global skip
#global testerror

struct A
{
    y: s32
    x: *u32
}

//#[Swag.PrintBc]
#[Swag.Optimize(.All, false)]
#test
{
    var x : A
    #[Swag.Safety(.All, .All, true)]
    var ptr = x.x[1]
}