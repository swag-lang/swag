struct AA
{
    b:   bool
}

//#[Swag.PrintBc]
func toto(x, y: typeinfo)->AA
{
    let b = x == y
    return {b}
}

#test
{
    let x = toto(s32, s32)
}
