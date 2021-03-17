var G = 0

#[swag.macro]
func toto(stmt2: code)
{
    func tata()
    {
        #mixin stmt2
    }

    tata()
}

#[swag.macro]
func titi(stmt1: code)
{
    toto()
    {
        G += 1
        #mixin stmt1
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