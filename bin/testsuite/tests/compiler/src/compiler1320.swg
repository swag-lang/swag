#[Swag.Macro]
func toto(x: #code void)
{
    #inject(x)
    #inject(x)
}

//#[Swag.PrintBc]
#test
{
    var cpt = 0
    toto(#code { #up cpt += 1; })
    @assert(cpt == 2)
}
