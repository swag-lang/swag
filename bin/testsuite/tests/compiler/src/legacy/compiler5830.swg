//#[Swag.PrintBc]
func toto(str: string, x: cvarargs)
{
    var valist: Swag.CVaList
    @cvastart(&valist)
    let a = @cvaarg(&valist, s32)
    let b = @cvaarg(&valist, s32)
    //let c = @cvaarg(&valist, f64)
    //@print(a, " ", b, "\n")
    //@assert(a == 111)
    @assert(b == 222)
    //@assert(c == 3.14)
    @cvaend(&valist)
}

//#[Swag.PrintBc]
#test
{
    toto("oula", 111, 222, 3.14)
}
