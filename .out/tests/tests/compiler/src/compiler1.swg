func level0()
{
    titi := 666's64
    @print(titi)
}

//#[swag.printbc]
#[swag.optim("", false)]
func test(level: int)
{
    @bcdbg()
    ptr := &level
    level0()
    toto := level
    @print(cast(s64) toto)
}

#test
{
    test(5)
}