using Swag

var G = 0

#[mixin]
func toto()
{
    func titi()
    {
        G += 1
    }

    titi()
}

//#[Swag.printbc]
#test
{
    toto()
    //toto()
    @assert(G == 1)
}