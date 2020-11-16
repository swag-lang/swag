struct A
{
    x = 1
}

//#[swag.printbc]
func toto1()->[2] A
{
    var result: retval
    return result
}

#test
{
    res := toto1()
    @assert(res[0].x == 1)
    @assert(res[1].x == 1)
}