struct Err {}

func toto() throw
{
    throw Err{}
}

//#[Swag.PrintBc]
func titi()->s32
{
    trycatch toto()
    @assert(false)
    return 666
}

#test
{
    let x = titi()
    @assert(x == 0)
}
