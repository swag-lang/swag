using Core, Pixel

#test
{
    var t: Poly2Tri
    t.startPolyLine()
    t.addPoint(0, 0)
    t.addPoint(100, 0)
    t.addPoint(100, 100)
    t.endPolyLine()
    t.triangulate()
}