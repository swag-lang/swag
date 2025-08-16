using Core, Pixel
using Core.Serialization

struct AA
{
    #[Serialization.Compress]
    img: Image
}

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'Write.TagBin

    var x: AA
    x.img = try Image.load(imagePath("capture.png"))

    var buf: ConcatBuffer
    {
        //Time.Stopwatch.scopeMeasure()
        try enc.writeAll(&buf, x)
    }
    buf.makeLinear()
    //@print(@countof(buf))

    var dec: Decoder'Read.TagBin
    let x1   = try dec.readAll'AA(buf.toSlice())
    defer Memory.delete(x1)

    let res3 = ImageStatistics.haarCompareTo(x.img, x1.img)
    @assert(res3 > 0.99)
}
