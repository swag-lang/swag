struct Point
{
    x:     s32 = 666
    y:     s32
}

#test
{
    var cpt = Point{}
    @assert(cpt.x == 666)
    @assert(cpt.y == 0)
}
