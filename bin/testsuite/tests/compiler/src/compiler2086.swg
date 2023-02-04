struct BB
{
    alias x = {u32}
    alias y = {u32, u32}
}

#test
{
    var v: any
    v = BB.x{5}

    switch @kindof(v)
    {
    case BB.x:
        vv := cast(BB.x) v
        @assert(vv.item0 == 5)
    default:
        @assert(false)
    }

    v = BB.y{5, 10}

    switch @kindof(v)
    {
    case BB.y:
        vv := cast(BB.y) v
        @assert(vv.item0 == 5)
        @assert(vv.item1 == 10)
    default:
        @assert(false)
    }
}
