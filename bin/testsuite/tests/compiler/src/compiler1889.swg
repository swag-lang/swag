
#global testerror "Err0054"

//#[Swag.PrintBc]
func toto(x: s64)
{
    //@assert(x == 666)
}

//#[Swag.PrintBc]
func titi()
{
    var xyz = 666's64
    #run toto(xyz)
}
