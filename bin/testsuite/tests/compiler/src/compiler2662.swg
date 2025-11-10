var G = 0

#[Swag.Macro]
func toto(stmt: #code void)
{
    func tata()
    {
        #inject(stmt)
    }

    tata()
}

#[Swag.Macro]
func titi(stmt: #code void)
{
    toto()
    {
        G += 1
        #inject(stmt)
    }
}

#test
{
    titi()
    {
        G += 2
    }

    @assert(G == 3)
}
