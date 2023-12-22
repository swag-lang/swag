#global testerror "Err0206"
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
#[Swag.Warn("wrn0009", Swag.WarnLevel.Disable)]
#test
{
    var x = 5's64
    toto(x)
    {
        G += 1
    }

    @assert(G == 10)
}