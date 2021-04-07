func level0()
{
    @bcdbg()
}

//#[swag.printbc]
func test(level: int)
{
    if (level > 0)
    {
        prevLevel := level - 1;
        test(prevLevel);
    }
    else
        level0();
}

#test
{
    test(5)
}