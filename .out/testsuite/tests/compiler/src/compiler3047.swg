struct AA
{
    b: bool
}

//#[Swag.PrintBc]
func toto(x, y: typeinfo)->AA
{
    b := x == y
    return @{b}
}

#test
{
    x := toto(s32, s32)
}