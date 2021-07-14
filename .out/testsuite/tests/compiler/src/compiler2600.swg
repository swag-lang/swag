#global testerror
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

    loop 10 titi()
}

//#[PrintBc]
#test
{
    x := 5's64
    toto(x)
    {
        G += 1
    }

    @assert(G == 10)
}