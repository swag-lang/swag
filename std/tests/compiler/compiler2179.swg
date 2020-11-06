struct A
{
    x: u32
}

//#[swag.printbc]
func toto()->A
{
    result := retval
    result.x = 10
    return result
}

//#[swag.printbc]
#test
{
    v := toto()
    @assert(v.x == 10)
}