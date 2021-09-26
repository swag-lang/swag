#[Swag.Discardable]
func printf(format: cstring, args: cvarargs)->int
{
    var valist: Swag.CVaList
    @cvastart(&valist)
    x := @cvaarg(&valist, f64)
    //@print(x)
    @assert(x == 3.14)
    @cvaend(&valist)
    return 0
}

#test
{
    printf("", 3.14)
}
