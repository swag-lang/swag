using swag

#test
{
    @assert(toto() == 556)
    @assert(toto() == 557)
}

func toto()->s32
{
    #[global] var x = 555
    x += 1
    return x
}