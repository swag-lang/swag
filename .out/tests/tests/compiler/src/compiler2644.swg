var G = 0

#[swag.macro]
func titi(stmt: code)
{
    #mixin stmt
    G *= 2
}

#[swag.macro]
func toto(stmt: code)
{
    titi(stmt)
}

//#[swag.printbc]
#test
{
    toto()
    {
        G = 1
    }

    @assert(G == 2)
}