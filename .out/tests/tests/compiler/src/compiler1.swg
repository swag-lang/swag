func level0()
{
}

//#[swag.printbc]
func test(level: int)
{
    if (level > 0)
    {
        @bcdbg()
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