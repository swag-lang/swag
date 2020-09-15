#import "std"

using std.collections, std.diagnostics, std.text

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
    format(&buf, "%{ABC}toto", x)
    @assert(cast(string) buf.toString() == "toto")

    format(&buf, "{ABC}toto", x)
    @assert(cast(string) buf.toString() == "{ABC}toto")
}
