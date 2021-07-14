var G = 0

#[Swag.Macro]
func toto(stmt: code)
{
    func tata()
    {
        #mixin stmt
    }

    tata()
}

#[Swag.Macro]
func titi(stmt: code)
{
    toto()
    {
        G += 1
        #mixin stmt
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