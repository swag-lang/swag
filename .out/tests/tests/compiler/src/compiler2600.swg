#global testerror
using Swag

var G = 0

#[macro]
func toto(cpt: s64, stmt: code)
{
    //#[printbc]
    func titi()
    {
        @print(cpt)
        #mixin stmt
    }

    loop 10 titi()
}

//#[printbc]
#test
{
    x := 5's64
    toto(x)
    {
        G += 1
    }

    @assert(G == 10)
}