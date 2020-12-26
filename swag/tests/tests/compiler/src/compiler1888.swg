//#[swag.printbc]
func toto(x: s64)
{
    @assert(x == 666)
}

//#[swag.printbc]
func titi()
{
    const x = 666's64
    #run toto(x)
}
