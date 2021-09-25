//#[Swag.PrintBc]
func toto(str: string, x: cvarargs)
{
    var valist: Swag.CVaList
    @cvastart(&valist)
    a := @cvaarg(&valist, s32)
    b := @cvaarg(&valist, s32)
    c := @cvaarg(&valist, f32)
    //@print(a, " ", b, " ", c, "\n")
    @assert(a == 111)
    @assert(b == 222)
    @assert(c == 3.14)
    @cvaend(&valist)
}

//#[Swag.PrintBc]
#test
{
    toto("oula", 111, 222, 3.14)
}