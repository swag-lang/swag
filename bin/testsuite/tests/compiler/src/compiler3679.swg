struct A
{
    x: [3] s32
}

impl A
{
    mtd opIndexAffect(index0, index1, index2: u64, value: s32)
    {
        @assert(index0 == 4)
        @assert(index1 == 40)
        @assert(index2 == 400)
        @assert(value == 666)
        x[2] = 666
    }
}

#test
{
    var a: A
    a[4, 40, 400] = 666
    @assert(a.x[2] == 666)
}