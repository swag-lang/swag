using swag

var G = 0

#[inline]
func toto()
{
    func titi()
    {
        G = 1
    }

    titi()
}

//#[swag.printbc]
#test
{
    toto()
    @assert(G == 1)
}