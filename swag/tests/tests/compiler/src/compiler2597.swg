using swag

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

//#[swag.printbc]
#test
{
    toto()
    //toto()
    @assert(G == 1)
}