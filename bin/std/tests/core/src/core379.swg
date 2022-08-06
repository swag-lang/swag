using Core, Serialization, Math

struct Toto1
{
    x: Array'Vector3
}

struct Toto2
{
    x: Vector3
}

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'Write.TagBin

    var buf: ConcatBuffer
    enc.start(&buf)
    enc.IEncoder.beginSection(.Unordered)
    enc.IEncoder.endSection()
    enc.end()
    buf.makeLinear()

    var dec: Decoder'Read.TagBin
    try dec.start(buf.toSlice())
    try dec.IDecoder.beginSection(.Unordered)
    try dec.IDecoder.endSection()
    try dec.end()
}