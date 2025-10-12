#global testerror "Saf0004"

struct A
{
    y:     s32
    x:     string
}

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#test
{
    let x: A
    #[Swag.Safety(.All, .All, true)]
    var ptr = x.x[1]
}
