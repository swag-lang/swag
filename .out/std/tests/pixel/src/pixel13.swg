using Core, Pixel, Clipper

#test
{
    clipper := Transform{}

    using Math
    lp := Array'Vector2{}
    lp.add(Vector2{0, 0})
    lp.add(Vector2{100, 0})
    lp.add(Vector2{100, 100})
    lp.add(Vector2{0, 100})
    assume clipper.addPath(lp, PolyType.Subject)

    var lpl: Polygon
    assume clipper.execute(ClipType.Union, &lpl, PolyFillType.NonZero, PolyFillType.NonZero)
    @assert(lpl.points.count == 5)
}