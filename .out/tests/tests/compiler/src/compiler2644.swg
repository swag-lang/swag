var G = 0

#[Swag.macro]
func titi(stmt: code)
{
    #mixin stmt
    G *= 2
}

#[Swag.macro]
func toto(stmt: code)
{
    titi(stmt)
}

//#[Swag.printbc]
#test
{
    toto()
    {
        G = 1
    }

    @assert(G == 2)
}