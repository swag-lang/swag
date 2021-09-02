#global testerror

func toto(x = @[1, 2])
{
    @assert(x[0] == 1)
    @assert(x[1] == 2)
}

//#[Swag.PrintBc]
#test
{
    toto()
    toto(@[1, 2])
}