using swag

var G = 0

#[macro]
func toto()
{
    func titi()
    {
        G += 1
    }

    titi()
}

//#[swag.printbc]
#test
{
    toto()
    toto()
    @assert(G == 2)
}