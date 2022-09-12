#global testerror

func level0()
{
    titi := 666's64
    //@print(titi)
}


//#[Swag.PrintBc]
#[Swag.Optim("", false)]
func test(level: int)
{
    //@breakpoint()
    ptr := &level
    level0()
    toto := level
    //@print(cast(s64) toto)
}

#test
{
    test(5)
}