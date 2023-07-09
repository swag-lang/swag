struct AA
{
    b: bool
}

//#[Swag.PrintBc]
func toto(x, y: typeinfo)->AA
{
    var b = x == y
    return {b}
}

#test
{
    var x = toto(s32, s32)
}