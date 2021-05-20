
#global testerror

//#[Swag.printbc]
func toto(x: s64)
{
    //@assert(x == 666)
}

//#[Swag.printbc]
func titi(x: s64)
{
    #run
    {
        toto(x)
    }
}
