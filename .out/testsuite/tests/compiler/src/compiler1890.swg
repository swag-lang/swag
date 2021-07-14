
#global testerror

//#[Swag.PrintBc]
func toto(x: s64)
{
    //@assert(x == 666)
}

//#[Swag.PrintBc]
func(T) titi()
{
    xyz := 666's64
    #run toto(xyz)
}

#test
{
    titi's32()
}