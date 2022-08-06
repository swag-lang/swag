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
    enc.serializer.beginSection()
    enc.serializer.endSection()
    enc.end()
    buf.makeLinear()

    var dec: Decoder'Read.TagBin
    try dec.start(buf.toSlice())
    try dec.serializer.beginSection()
    try dec.serializer.endSection()
    try dec.end()
}