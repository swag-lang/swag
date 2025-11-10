//#[Swag.PrintBc]
func toto1()->[2] s32
{
    var result: retval = [1, 2]
    return result
}

#test
{
    var res = toto1()
    @assert(res[0] == 1)
    @assert(res[1] == 2)
}
