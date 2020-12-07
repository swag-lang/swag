//#[swag.printbc]
func toto(x: s64)
{
    @assert(x == 666)
}

//#[swag.printbc]
func titi(x: bool)
{
    #run
    {
        x := 666's64
        toto(x)
    }
}
