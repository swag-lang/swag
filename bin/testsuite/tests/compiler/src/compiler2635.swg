//#[Swag.PrintBc]
func toto()->s32
{
    let result: retval = 10
    return result
}

#test
{
    @assert(toto() == 10)
}
