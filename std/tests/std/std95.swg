#import "std"
using std

//#[swag.printbc]
#test
{
    var buf: ConcatBuffer
    format.append(&buf, "%", false)
    @assert(buf.toString() == "false")
    format.append(&buf, "%", true)
    @assert(buf.toString() == "true")

    format.append(&buf, "%", "1"'char)
    @assert(buf.toString() == "1")

    //format(&buf, "%", &buf)

    //str := buf.toString()
    //console.write(cast(string) str)
}
