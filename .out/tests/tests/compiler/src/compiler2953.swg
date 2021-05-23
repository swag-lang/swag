struct AA
{
    x: s32
    y: s32 = 1
}

var ggg: AA

#test
{
    @assert(ggg.y == 1)
}