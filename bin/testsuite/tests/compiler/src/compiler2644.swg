var G = 0

#[Swag.Macro]
func titi(stmt: #code void)
{
    #inject(stmt)
    G *= 2
}

#[Swag.Macro]
func toto(stmt: #code void)
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