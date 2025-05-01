#global testerror "Err0319"

#[Swag.Macro]
func toto(x: #block)
{
    #inject x
    #inject x
}

//#[Swag.PrintBc]
#test
{
    var cpt = 0
    toto(#code { #up cpt += 1; })
    @assert(cpt == 2)
}