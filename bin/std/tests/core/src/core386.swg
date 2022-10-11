#global testerror
using Core.Serialization.Write

struct A
{
    v: *void
}

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'TagBin
    var buf: ConcatBuffer
    var v: A
    try enc.writeAll(&buf, v)
}