

using core, core.StrConv

struct TOTO
{
    x: s32 = 555
    y: s32 = 666
}

impl IConvert for TOTO
{
    func convert(using self, buf: *ConcatBuffer, fmt: const *ConvertFormat, strFormat: string)
    {
        buf.addBytes("! x = ");
        convertInt(buf, x, null)
        buf.addBytes(", y = ");
        convertInt(buf, y, null)
        buf.addBytes(" !");
    }
}

#test
{
    var buf: ConcatBuffer

    var x: TOTO
    Format.append(&buf, "%", x)
    //@print(cast(string) buf.toString())
    @assert(buf.toString() == "! x = 555, y = 666 !")
}
