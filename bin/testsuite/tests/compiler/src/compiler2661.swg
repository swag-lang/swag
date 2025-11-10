var G = 0

#[Swag.Macro]
func toto(stmt2: #code void)
{
    func tata()
    {
        #inject(stmt2)
    }

    tata()
}

#[Swag.Macro]
func titi(stmt1: #code void)
{
    toto()
    {
        G += 1
        #inject(stmt1)
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
