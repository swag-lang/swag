
#global testerror

//#[Swag.PrintBc]
func toto(x: s64)
{
    //@assert(x == 666)
}

//#[Swag.PrintBc]
func titi(x: s64)
{
    #run
    {
        toto(x)
    }
}
