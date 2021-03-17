const A = false

struct E
{
    #if A
    {
        x: s32 = 10
    }
    #else
    {
        y: s32 = 10
    }
}

#test
{
    var pt: E
    @assert(pt.y == 10)
    @assert(@sizeof(pt) == 4)
}