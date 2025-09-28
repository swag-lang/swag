struct Point { x, y, z = 4 }

#test
{
    var pt = Point{}
    @assert(pt.x + pt.y + pt.z == 12)
}
