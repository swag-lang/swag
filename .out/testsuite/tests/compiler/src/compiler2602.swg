using Swag

var G = 0

#[macro]
func toto(stmt: code)
{
    func titi(cpt: s32)
    {
        var @alias0 = cpt
        #mixin stmt
    }

    loop 5 titi(@index)
    loop 5 titi(@index)
}

//#[Swag.PrintBc]
#test
{
    toto()
    {
        G += @alias0
    }

    @assert(G == 20)
}