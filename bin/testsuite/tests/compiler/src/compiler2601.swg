using Swag

var G = 0

#[Macro]
func toto(stmt: code)
{
    func titi(cpt: s32)
    {
        var #alias0 = cpt
        #inject stmt
    }

    for 5: titi(#index)
    for 5: titi(#index)
}

//#[Swag.PrintBc]
#test
{
    toto()
    {
        G += #alias0
    }

    @assert(G == 20)
}