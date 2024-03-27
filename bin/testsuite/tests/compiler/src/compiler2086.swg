struct BB
{
    typealias x = {a: u32}
    typealias y = {a, b: u32}
}

#test
{
    var v: any
    v = BB.x{5}

    switch v
    {
    case BB.x:
        var vv = cast(BB.x) v
        @assert(vv.item0 == 5)
    default:
        @assert(false)
    }

    v = BB.y{5, 10}

    switch v
    {
    case BB.y:
        var vv = cast(BB.y) v
        @assert(vv.item0 == 5)
        @assert(vv.item1 == 10)
    default:
        @assert(false)
    }
}
