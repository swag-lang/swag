func toto(str: string, x: cvarargs)
{
    var valist: Swag.CVaList
    @cvastart(&valist)
    @cvaend(&valist)
}

//#[Swag.PrintBc]
#test
{
    toto("oula", 111, 222)
}
