#import "std"

using std.collections, std.diagnostics, std.text

struct TOTO
{
    x: s32 = 555
    y: s32 = 666
}

impl IConvert for TOTO
{
    func convert(using self, buf: *ConcatBuffer)
    {
        buf.addBytes("{ x = ");
        convertInt(buf, x)
        buf.addBytes(", y = ");
        convertInt(buf, y)
        buf.addBytes(" }");
    }
}

//#[swag.printbc]
#test
{
    var buf: ConcatBuffer

    var x: TOTO
    format(&buf, "%", x)
    //@print(cast(string) buf.toString())
    @assert(buf.toString() == "{ x = 555, y = 666 }")
}
