func toto()->s32
{
    return 56
}

//#[Swag.PrintBc]
#test
{
    let x = [toto()] //, toto()]
    @assert(x[0] == 56)
    //@assert(x[1] == 56)
}
