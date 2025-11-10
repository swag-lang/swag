using Swag

var G = 0

#[Macro]
func toto(stmt: #code void)
{
    func titi()
    {
        G += 1
    }

    titi()
    #inject(stmt)
}

//#[Swag.PrintBc]
#test
{
    toto()
    {
        titi()
    }

    @assert(G == 2)
}
