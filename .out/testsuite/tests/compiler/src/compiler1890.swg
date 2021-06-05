
#global testerror

//#[Swag.printbc]
func toto(x: s64)
{
    //@assert(x == 666)
}

//#[Swag.printbc]
func(T) titi()
{
    xyz := 666's64
    #run toto(xyz)
}

#test
{
    titi's32()
}