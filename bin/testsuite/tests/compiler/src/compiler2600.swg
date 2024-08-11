#global testerror "Err0056"
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

    loop 10 do titi()
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