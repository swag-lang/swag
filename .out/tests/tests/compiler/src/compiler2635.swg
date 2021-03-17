//#[swag.printbc]
func toto()->s32
{
    var result: retval = 10
    return result
}

#test
{
    @assert(toto() == 10)
}