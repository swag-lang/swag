using Swag

var G = 0

#[Macro]
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