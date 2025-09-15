//#global testerror
using Core.Serialization.Write

struct A
{
    v:   *void
}

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'TagBin
    var buf: ConcatBuffer
    enc.errorIfUnknown = true
    let v: A
    catch enc.writeAll(&buf, v)
    @assert(@err != null)
}
