//#[Swag.PrintBc]
func toto(x: s64)
{
    @assert(x == 666)
}

//#[Swag.PrintBc]
func titi()
{
    const x = 666's64
    #run toto(x)
}
