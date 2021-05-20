

using Core, Core.StrConv

struct TOTO
{
    x: s32 = 555
    y: s32 = 666
}

impl IConvert for TOTO
{
    func convert(using self, buf: *ConcatBuffer, fmt: const *ConvertFormat, strFormat: string)
    {
        @assert(strFormat == "ABC")
    }
}

#test
{
    var buf: ConcatBuffer

    var x: TOTO
    Format.append(&buf, "%{ABC}toto", x)
    @assert(cast(string) buf.toString() == "toto")

    buf.clear()
    Format.append(&buf, "{ABC}toto")
    @assert(cast(string) buf.toString() == "{ABC}toto")
}
