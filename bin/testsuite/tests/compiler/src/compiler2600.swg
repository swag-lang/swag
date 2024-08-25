#global testerror "Err0057"
using Swag

var G = 0

#[Macro]
func toto(cpt: s64, stmt: code)
{
    //#[PrintBc]
    func titi()
    {
        @print(cpt)
        #mixin stmt
    }

    for 10: titi()
}

//#[PrintBc]
#[Swag.Warning("Wrn0003", Swag.WarningLevel.Disable)]
#test
{
    var x = 5's64
    toto(x)
    {
        G += 1
    }

    @assert(G == 10)
}