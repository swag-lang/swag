using Swag

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

//#[Swag.PrintBc]
#test
{
    toto()
    toto()
    @assert(G == 2)
}