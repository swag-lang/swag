#global testerror "Err0081"

//#[Swag.PrintBc]
func toto(x: s64)
{
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
