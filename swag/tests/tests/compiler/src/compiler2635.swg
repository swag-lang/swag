//#[swag.printbc]
func toto()->s32
{
    var result: retval = 10
    result = 20
    return result
}

#test
{
    @assert(toto() == 20)
}