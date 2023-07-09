#[Swag.Discardable]
func printf(format: cstring, args: cvarargs)->s64
{
    var valist: Swag.CVaList
    @cvastart(&valist)
    var x = @cvaarg(&valist, s32)
    var y = @cvaarg(&valist, s32)
    @assert(x == 666)
    @assert(y == 555)
    @cvaend(&valist)
    return 0
}

#test
{
    printf("", 666, 555)
}
