var G = 0

#[swag.macro]
func toto(stmt: code)
{
    func tata()
    {
        #mixin stmt
    }

    tata()
}

#[swag.macro]
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