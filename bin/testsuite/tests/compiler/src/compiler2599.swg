using Swag

var G = 0

#[Macro]
func toto(stmt: code)
{
    func titi()
    {
        #mixin stmt
    }

    for 10: titi()
}

//#[Swag.PrintBc]
#test
{
    toto()
    {
        G += 1
    }

    @assert(G == 10)
}