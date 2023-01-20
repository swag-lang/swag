//#[Swag.PrintBc]
func toto(y: s8)->s8
{
    x := y
    x /= 3
    return x
}

#test
{
    y := toto(6)
    @assert(y == 2)
}

//#[Swag.PrintBc]
func toto(y, z: s8)->s8
{
    x1 := y
    x2 := z
    x1 /= x2
    return x1
}

#test
{
    y := toto(9, 3)
    @assert(y == 3)
}