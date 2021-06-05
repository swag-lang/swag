union Evt1
{
    x, y, z: s32
    b: bool
    v: s64
}

#assert @sizeof(Evt1) == 8

#test
{
    var v: Evt1
    @assert(!v.b)
    v.x = 1
    @assert(v.b)
}