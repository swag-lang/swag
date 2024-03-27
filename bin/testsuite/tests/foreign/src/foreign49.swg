public struct TT49
{
    x, y: s32
}

public func foufou49(a: string, b: s32, x: cvarargs)->TT49
{
    @assert(a == "string")
    @assert(b == 666)

    var valist: Swag.CVaList
    @cvastart(&valist)
    @assert(@cvaarg(&valist, f64) == 10)
    @assert(@cvaarg(&valist, f64) == 20)
    @assert(@cvaarg(&valist, f64) == 30)
    @cvaend(&valist)

    return {444, 555}
}
