public struct T40
{
    x: s32 = 555
    y: s32 = 666
}

public func toto40(f: any)
{
    @assert(@kindof(f) == T40)
    var ptr = cast(*T40) @dataof(f)
    @assert(ptr.x == 555)
    @assert(ptr.y == 666)
}
