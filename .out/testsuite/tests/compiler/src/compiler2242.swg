
#global testerror

func mix(v, val: alias)
{
    v += val
}


//#[Swag.printbc]
#test
{
    z := 4
    x, y := 0
    mix(x, z)
    mix(y, z)
    @assert(x == 4)
    @assert(y == 4)
}