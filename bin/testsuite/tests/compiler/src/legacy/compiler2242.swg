
#global testerror "Err0528"

func mix(v, val: alias)
{
    v += val
}


//#[Swag.PrintBc]
#test
{
    var z = 4
    var x, y = 0
    mix(x, z)
    mix(y, z)
    @assert(x == 4)
    @assert(y == 4)
}