using swag

var G = 0

#[macro]
func toto(stmt: code)
{
    func titi()
    {
        #mixin stmt
    }

    loop 10 titi()
}

//#[swag.printbc]
#test
{
    toto()
    {
        G += 1
    }

    @assert(G == 10)
}