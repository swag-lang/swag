#[swag.inline]
func toto1()->[2] s32
{
    var result: retval
    result[0] = 1
    result[1] = 2
    return result
}

#test
{
    res := toto1()
    @assert(res[0] == 1)
    @assert(res[1] == 2)
}