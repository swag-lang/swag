public func foufou47(a: string, b: s32, x: cvarargs)
{
    @assert(a == "string")
    @assert(b == 666)

    var valist: Swag.CVaList
    @cvastart(&valist)
    @assert(@cvaarg(&valist, s32) == 10)
    @assert(@cvaarg(&valist, s32) == 20)
    @assert(@cvaarg(&valist, s32) == 30)
    @cvaend(&valist)
}
