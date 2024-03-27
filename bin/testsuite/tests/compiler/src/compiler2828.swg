#global testerror "Err0181"

func level0()
{
    var titi = 666's64
}

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
func test(level: s64)
{
    //@breakpoint()
    var ptr = &level
    level0()
    var toto = level
}

#test
{
    test(5)
}
