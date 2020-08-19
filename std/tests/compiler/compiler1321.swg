#module "compiler1321"
#unittest error

#[swag.mixin]
func toto(x: code)
{
    #mixin x
    #mixin x
}

//#[swag.printbc]
#test
{
    cpt := 0
    toto(#code { `cpt += 1; })
    @assert(cpt == 2)
}