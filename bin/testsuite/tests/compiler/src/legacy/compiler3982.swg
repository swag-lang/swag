var G: string = "G"

func toto(x: const &string)
{
    @assert(x == "G")
}

func titi()->const &string => &G

//#[Swag.PrintBc]
#test
{
    toto(titi())
}
