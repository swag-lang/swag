var G: string

func toto(x: const ref string)
{
    @assert(x == "G")
}

//#[Swag.PrintBc]
#test
{
    toto("G")
}
