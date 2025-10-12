#global testerror "Saf0018"

alias A = string

//#[Swag.PrintGenBc]
#[Swag.Optimize("", false), Swag.Safety(.All, "", true), Swag.Sanity(true)]
func toto(b: s32)
{
    var x: A
    var y  = x[b to 1]
}

func test()
{
    toto(2)
}
