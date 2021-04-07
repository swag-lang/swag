func level0()
{
    titi := 666's64
    @print(titi)
}

//#[swag.printbc]
func test(level: int)
{
    @bcdbg()
    level0()
    toto := level
    @print(cast(s64) toto)
}

#test
{
    test(5)
}