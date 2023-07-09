#global skip // remove typeset from language the 21 july 2021
typeset AA
{
    T  {x: s32}
    T1 {x: s32, y: f32}
}

//#[Swag.PrintBc]
#test
{
    var v: AA

    {
        v = AA.T{666}
        var ptr = @kindof(v)
        @assert(ptr == AA.T)
        var vv = cast(AA.T) v
        @assert(vv.x == 666)
    }

    {
        v = AA.T1{123, 456}
        var ptr = @kindof(v)
        @assert(ptr == AA.T1)
        var vv = cast(AA.T1) v
        @assert(vv.x == 123)
        @assert(vv.y == 456)
    }
}