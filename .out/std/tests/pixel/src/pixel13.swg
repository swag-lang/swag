using Core, Pixel

#test
{
    clipper := Clipper{}

    lp := LinePath{}
    lp.start(0, 0)
    lp.lineTo(100, 0)
    lp.lineTo(100, 100)
    lp.lineTo(0, 100)
    lp.close()

    lp.flatten(PaintQuality.Normal)
    assume clipper.addPath(&lp, PolyType.Subject)

    var lpl: LinePathList
    assume clipper.execute(ClipType.Union, &lpl, PolyFillType.NonZero, PolyFillType.NonZero)
    @assert(lpl.paths.count == 1)
    @assert(lpl.paths[0].posBuffer.count == 4)
}