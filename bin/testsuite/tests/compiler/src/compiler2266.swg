struct Evt1
{
    params: union
    {
        two: struct
        {
            x: s32
            y: s32
        }

        three: struct
        {
            x: s32
            y: s32
            z: s32
        }
    }
}

#assert @sizeof(Evt1) == 12

#test
{
    var v: Evt1
    @assert(v.params.three.x == 0)
    v.params.two.x = 1
    @assert(v.params.three.x == 1)

    @assert(v.params.three.y == 0)
    v.params.two.y = 10
    @assert(v.params.three.y == 10)
}