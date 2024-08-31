#global testerror "Err0730"

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
    #macro
    {
        #mixin #up stmt
    }
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