struct Point
{
    x    = 4
}

#test
{
    var pt = Point{}
    @assert(pt.x == 4)
}
