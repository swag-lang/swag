struct V2 { x, y: s32 }

#test
{
    var a: V2
    a.x = 1

    var f: func||()
    f = func|a|()
    {
        @assert(a.x == 1)
    }

    f()
}
