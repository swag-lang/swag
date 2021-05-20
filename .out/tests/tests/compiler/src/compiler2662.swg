var G = 0

#[Swag.macro]
func toto(stmt: code)
{
    func tata()
    {
        #mixin stmt
    }

    tata()
}

#[Swag.macro]
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