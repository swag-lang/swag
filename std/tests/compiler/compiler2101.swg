typeset AA
{
    struct T {x: s32;}
    //y: {s32, f32}
}

//#[swag.printbc]
#test
{
    var v: AA
    v = AA.T{666}
    ptr := @kindof(v)

    @print(ptr.name)
    @assert(ptr == AA.T)
    //vv := cast(AA.T) v
}