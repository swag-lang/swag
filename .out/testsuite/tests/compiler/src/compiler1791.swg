using Swag

#test
{
    @assert(toto() == 556)
    @assert(toto() == 557)
}

func toto()->s32
{
    #[Global] var x = 555
    x += 1
    return x
}