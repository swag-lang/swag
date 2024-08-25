
#global testerror "Saf0004"

struct A
{
    y: s32
    x: string
}

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
#test
{
    var x : A
    #[Swag.Safety("", true)]
    var ptr = x.x[1]
}