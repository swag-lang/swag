

using Core

struct TOTO
{
    x: s32 = 555
    y: s32 = 666
}

//#[Swag.PrintBc]
#test
{
    var buf: ConcatBuffer

    var x: TOTO
    Format.append(&buf, "%", x)
    //@print(cast(string) buf.toString())
    @assert(buf.toString() == "{x = 555, y = 666}")
}
