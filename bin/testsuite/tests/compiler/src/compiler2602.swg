using Swag

var G = 0

#[Macro]
func toto(stmt: #code void)
{
    func titi(cpt: s32)
    {
        var #alias0 = cpt
        #inject(stmt)
    }

    for 5 do
        titi(@index)
    for 5 do
        titi(@index)
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
