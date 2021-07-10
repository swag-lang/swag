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

    lp.clear()
    lp.add(Vector2{50, 50})
    lp.add(Vector2{150, 50})
    lp.add(Vector2{150, 150})
    lp.add(Vector2{50, 150})
    assume clipper.addPath(lp, PolyType.Subject)

    var lpl: LinePathList
    assume clipper.execute(ClipType.Union, &lpl, PolyFillType.NonZero, PolyFillType.NonZero)
    @assert(lpl.paths.count == 1)
    @assert(lpl.paths[0].posBuffer.count == 8)
}