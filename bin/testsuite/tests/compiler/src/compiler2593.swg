using Swag

var G = 0

#[Macro]
func toto(stmt: code)
{
    func titi()
    {
        G += 1
    }

    titi()
    #mixin stmt
}

//#[Swag.PrintBc]
#test
{
    toto()
    {
        titi()
    }

    @assert(G == 2)
}