#global testerror "Err0244"

//#[Swag.PrintBc]
func toto(str: string, x: cvarargs)
{
    var valist: Swag.CVaList
    @cvastart(&valist)
    let a = @cvaarg(&valist, string)
}

//#[Swag.PrintBc]
#test
{
    toto("oula", 111, 222, 3.14)
}