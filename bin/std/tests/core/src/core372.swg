using Core, Core.Serialization, Core.Math

struct Toto1
{
    a: s32 = 266
    b: [4] s32 = [1, 2, 3, 4]
    c: [4] f32 = [0, 1, 0, 1]
    d: [4] s8  = [-1, 0, 0, 0]
}

struct Toto2
{
    a: f32
    b: [4] f32
    c: [4] bool
    d: [4] u8
}

//#[Swag.PrintBc]
#test
{
    var enc: Encoder'Write.TagBin

    var x: Toto1
    var buf: ConcatBuffer
    try enc.writeAll(&buf, x)
    buf.makeLinear()

    var dec: Decoder'Read.TagBin
    let x1 = try dec.readAll'Toto2(buf.toSlice())
    defer Memory.delete(x1)

    //Console.print(dref x1)
    @assert(x1.a == 266)
    @assert(x1.b[0] == 1)
    @assert(x1.b[3] == 4)
    @assert(x1.c[0] == false)
    @assert(x1.c[1] == true)
    @assert(x1.d[0] == 255)
}