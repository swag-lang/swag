#import "std"
using std.text, std.collections

#test
{
    var buf: ConcatBuffer
    convertInt(&buf, 123's32)
}

#test
{
    var buf: ConcatBuffer
    convertInt(&buf, 123456's32)
    str := buf.toString()
    console.write(cast(string) str)
}