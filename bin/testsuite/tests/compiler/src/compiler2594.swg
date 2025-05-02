#global testerror "Err0679"

using Swag

var G = 0

#[Macro]
func toto(stmt: #code void)
{
    func titi()
    {
        G += 1
    }

    titi()
    #macro
    {
        #inject #up stmt
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