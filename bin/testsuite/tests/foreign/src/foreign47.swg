public func foufou47(a: string, b: s32, x: cvarargs)
{
    var valist: Swag.CVaList
    @cvastart(&valist)
    @assert(@cvaarg(&valist, s32) == 10)
    @assert(@cvaarg(&valist, s32) == 20)
    @assert(@cvaarg(&valist, s32) == 30)
    @cvaend(&valist)
}