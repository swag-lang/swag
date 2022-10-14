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
    enc.errorIfUnknown = true
    var v: A
    assume enc.writeAll(&buf, v)
}