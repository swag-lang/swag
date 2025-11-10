struct BB
{
    alias x = { a: u32 }
    alias y = { a, b: u32 }
}

#test
{
    var v: any
    v = BB.x{5}

    switch v
    {
    case BB.x:
        let vv = cast(BB.x) v
        @assert(vv.item0 == 5)
    default:
        @assert(false)
    }

    v = BB.y{5, 10}

    switch v
    {
    case BB.y:
        let vv = cast(BB.y) v
        @assert(vv.item0 == 5)
        @assert(vv.item1 == 10)
    default:
        @assert(false)
    }
}
