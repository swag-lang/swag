using Swag

var G = 0

#[Mixin]
func toto()
{
    func titi()
    {
        G += 1
    }

    titi()
}

//#[Swag.PrintBc]
#test
{
    toto()
    //toto()
    @assert(G == 1)
}