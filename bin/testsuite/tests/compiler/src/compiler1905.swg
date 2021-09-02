#test
{
    ptr := @typeof(s32)
    g := 0
    switch ptr
    {
        case s32:   g = 1
        default:    @assert(false)
    }

    @assert(g == 1)
}