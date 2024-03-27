//#[Swag.PrintBc]
#[Swag.Overload]
func toto(y: s8)->s8
{
    var x = y
    x /= 3
    return x
}

#test
{
    let y = toto(6)
    @assert(y == 2)
}

//#[Swag.PrintBc]
#[Swag.Overload]
func toto(y, z: s8)->s8
{
    var x1 = y
    let x2 = z
    x1 /= x2
    return x1
}

#test
{
    let y = toto(9, 3)
    @assert(y == 3)
}
