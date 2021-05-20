using Swag

var G = 0

#[macro]
func toto(stmt: code)
{
    func titi()
    {
        G += 1
    }

    titi()
    #mixin stmt
}

//#[Swag.printbc]
#test
{
    toto()
    {
        titi()
    }

    @assert(G == 2)
}