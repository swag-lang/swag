using Core, Pixel, Clipper

#test
{
    clipper := Transform{}

    using Math

    subj0 := Array'Vector2{}
    subj0.add(Vector2{180, 200}); subj0.add(Vector2{260,200}); subj0.add(Vector2{260,150}); subj0.add(Vector2{180,150})

    subj1 := Array'Vector2{}
    subj1.add(Vector2{215, 160}); subj1.add(Vector2{230,190}); subj1.add(Vector2{200,190})

    clip := Array'Vector2{}
    clip.add(Vector2{190, 210}); clip.add(Vector2{240,210}); clip.add(Vector2{240,130}); clip.add(Vector2{190,130})

    assume clipper.addPath(subj0, PolyType.Subject)
    assume clipper.addPath(subj1, PolyType.Subject)
    assume clipper.addPath(clip, PolyType.Clip)

    var lpl: LinePathList
    assume clipper.execute(ClipType.Intersection, &lpl, PolyFillType.NonZero, PolyFillType.NonZero)
    @assert(lpl.paths.count == 2)
}