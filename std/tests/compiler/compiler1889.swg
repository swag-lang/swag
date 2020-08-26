#module "compiler1889"
#unittest error

//#[swag.printbc]
func toto(x: s64)
{
    //@assert(x == 666)
}

//#[swag.printbc]
func titi()
{
    xyz := 666's64
    #run toto(xyz)
}
