var G: string = "G"

func toto(x: const ref string)
{
    @assert(x == "G")
}

func titi()->const ref string => &G

//#[Swag.PrintBc]
#test
{
    toto(titi())
}
