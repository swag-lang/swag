
#global testerror "Err0289"

func mix(v, val: typealias)
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