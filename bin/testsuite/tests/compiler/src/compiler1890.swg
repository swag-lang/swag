
#global testerror "Err0081"

//#[Swag.PrintBc]
func toto(x: s64)
{
    //@assert(x == 666)
}

//#[Swag.PrintBc]
func(T) titi()
{
    var xyz = 666's64
    #run toto(xyz)
}

#test
{
    titi's32()
}