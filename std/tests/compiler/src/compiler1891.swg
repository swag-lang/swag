#module "compiler1891"
#unittest error

//#[swag.printbc]
func toto(x: s64)
{
    //@assert(x == 666)
}

//#[swag.printbc]
func titi(x: s64)
{
    #run toto(x)
}
