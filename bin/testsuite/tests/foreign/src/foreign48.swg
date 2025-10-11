public func foufou48(a: string, b: s32, x: cvarargs)->s32
{
    @assert(a == "string")
    @assert(b == 666)

    var valist: Swag.CVaList
    @cvastart(&valist)
    @assert(@cvaarg(&valist, f64) == 10)
    @assert(@cvaarg(&valist, f64) == 20)
    @assert(@cvaarg(&valist, f64) == 30)
    @cvaend(&valist)

    return 555
}
