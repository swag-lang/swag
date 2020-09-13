#import "std"
using std.collections, std.text, std.diagnostics

func toto(v: String)
{
    //itf := @mkinterface(&v, @typeof(String), IConvert)

    //var buf: ConcatBuffer
    //if itf != null
    //    itf.convert(&buf, null)

    console.write("std119: %", v)
}

#test
{
    var x: String
    toto(x)
}