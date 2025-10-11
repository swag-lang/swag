var g = "titi"

func titi()->const &string
{
    return &g
}

func toto(x: const [..] u8)
{
    @assert(@countof(x) == 4)
}

//#[Swag.PrintBc]
#test
{
    toto(titi())
}
