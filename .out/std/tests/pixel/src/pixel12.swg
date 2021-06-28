using Core, Pixel

#test
{
    var t: Poly2Tri
    t.startPolyLine()
    t.addPoint(0, 0)
    t.addPoint(100, 0)
    t.addPoint(100, 100)
    t.addPoint(0, 100)
    t.endPolyLine()

    t.startPolyLine()
    t.addPoint(25, 25)
    t.addPoint(75, 25)
    t.addPoint(75, 75)
    t.addPoint(25, 75)
    t.endPolyLine()
    t.triangulate()

    res := t.getTriangles()
    @assert(@countof(res) == 8)
}