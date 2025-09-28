#[Swag.Discardable]
func printf(format: cstring, args: cvarargs)->s64
{
    var valist: Swag.CVaList
    @cvastart(&valist)
    let x = @cvaarg(&valist, f64)
    //@print(x)
    @assert(x == 3.14)
    @cvaend(&valist)
    return 0
}

#test
{
    printf("", 3.14)
}
