struct A
{
    x = 1
}

//#[Swag.PrintBc]
func toto1()->[2] A
{
    var result: retval
    return result
}

#test
{
    var res = toto1()
    @assert(res[0].x == 1)
    @assert(res[1].x == 1)
}