#import "std"
using std.collections, std.text

//#[swag.printbc]
#test
{
    var buf: ConcatBuffer
    format(&buf, "%", false)
    @assert(buf.toString() == "false")
    format(&buf, "%", true)
    @assert(buf.toString() == "true")

    format(&buf, "%", "1"'char)
    @assert(buf.toString() == "1")

    //format(&buf, "%", &buf)

    //str := buf.toString()
    //console.write(cast(string) str)
}
