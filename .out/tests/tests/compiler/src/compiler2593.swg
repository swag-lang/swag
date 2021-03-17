using swag

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

//#[swag.printbc]
#test
{
    toto()
    {
        titi()
    }

    @assert(G == 2)
}