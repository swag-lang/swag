#global skip
#global testerror

struct A
{
    y: s32
    x: *u32
}

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#test
{
    var x : A
    #[Swag.Safety(.All, "", true)]
    var ptr = x.x[1]
}