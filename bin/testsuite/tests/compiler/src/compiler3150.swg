#global if #os == "windows" and #arch == "x86_64"

func toto(x: cvarargs)
{
    var valist: Swag.CVaList
    @cvastart(&valist)

    ptr := dref cast(**s64) &valist
    //@assert(ptr[0] == 111)
    //@assert(ptr[1] == 222)
}

//#[Swag.PrintBc]
#test
{
    toto(111, 222)
}