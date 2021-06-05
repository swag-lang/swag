struct A
{
    x: u32
}

//#[Swag.printbc]
func toto()->A
{
    var result: retval
    result.x = 10
    return result
}

//#[Swag.printbc]
#test
{
    v := toto()
    @assert(v.x == 10)
}