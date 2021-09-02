//#[Swag.PrintBc]
func toto(x: s64)
{
    @assert(x == 666)
}

//#[Swag.PrintBc]
func titi(x: bool)
{
    #run
    {
        x := 666's64
        toto(x)
    }
}
