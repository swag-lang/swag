struct A
{
    x: u32
}

//#[Swag.PrintBc]
func toto()->A
{
    var result: retval
    result.x = 10
    return result
}

//#[Swag.PrintBc]
#test
{
    var v = toto()
    @assert(v.x == 10)
}