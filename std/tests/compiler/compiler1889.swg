#module "compiler1889"
#unittest error

//#[swag.printbc]
func toto(x: s64)
{
    @assert(x == 666)
}

//#[swag.printbc]
func titi()
{
    x := 666's64
    #run toto(x)
}