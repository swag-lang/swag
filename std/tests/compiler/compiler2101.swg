typeset AA
{
    T  {x: s32;}
    T1 {x: s32; y: f32;}
}

//#[swag.printbc]
#test
{
    var v: AA

    {
        v = AA.T{666}
        ptr := @kindof(v)
        @assert(ptr == AA.T)
        vv := cast(AA.T) v
        @assert(vv.x == 666)
    }

    {
        v = AA.T1{123, 456}
        ptr := @kindof(v)
        @assert(ptr == AA.T1)
        vv := cast(AA.T1) v
        @assert(vv.x == 123)
        @assert(vv.y == 456)
    }
}