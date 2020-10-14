typeset AA
{
    T  {x: s32;}
    T1 {x: s32; y: f32;}
}

//#[swag.printbc]
#test
{
    var v: AA
    v = AA.T{666}
    ptr := @kindof(v)

    //@print(ptr.name)
    @assert(ptr == AA.T)

    vv := cast(AA.T) v
    @assert(vv.x == 666)
}