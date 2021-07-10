using Core, Pixel

#test
{
    var t: Poly2Tri.Tesselate
    t.startPolyLine()
    t.addPoint(0, 0)
    t.addPoint(100, 0)
    t.addPoint(100, 100)
    t.endPolyLine()
    t.triangulate()

    res := t.getTriangles()
    @assert(res[0].points[0].x == 0)
    @assert(res[0].points[1].x == 100)
    @assert(res[0].points[2].x == 100)
}