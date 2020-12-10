#import "std"
using std

//#[swag.printbc]
func XX(v: String)->[..]u8 => @mkslice(v.buffer, cast(uint) v.length)

//#[swag.printbc]
#test
{
    var x: String = "coucou㐁㐄㐅"
    y := XX(x)
    console.silent(true)
    console.print(cast(string) y)
    console.silent(false)
}