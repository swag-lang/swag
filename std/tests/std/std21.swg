#import "std"
using std

//#[swag.printbc]
func XX(v: String)->[..]u8 => @mkslice(v.buffer, v.length)

//#[swag.printbc]
#test
{
    var x: String = "coucou㐁㐄㐅"
    y := XX(x)
    console.silent(true)
    console.write(cast(string) y)
    console.silent(false)
}