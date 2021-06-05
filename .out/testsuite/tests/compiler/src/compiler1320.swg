#[Swag.macro]
func toto(x: code)
{
    #mixin x
    #mixin x
}

//#[Swag.printbc]
#test
{
    cpt := 0
    toto(#code { `cpt += 1; })
    @assert(cpt == 2)
}