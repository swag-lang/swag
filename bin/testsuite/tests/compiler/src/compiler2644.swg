var G = 0

#[Swag.Macro]
func titi(stmt: code)
{
    #inject stmt
    G *= 2
}

#[Swag.Macro]
func toto(stmt: code)
{
    titi(stmt)
}

//#[Swag.PrintBc]
#test
{
    toto()
    {
        G = 1
    }

    @assert(G == 2)
}