#global testerror "Err0148"

func level0()
{
    var titi = 666's64
    //@print(titi)
}

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
func test(level: s64)
{
    //@breakpoint()
    var ptr = &level
    level0()
    var toto = level
    //@print(cast(s64) toto)
}

#test
{
    test(5)
}
