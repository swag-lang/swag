#import "std"
using std

func XX(v: String)->[..]u8 => [v.buffer, v.length]

//#[swag.printbc]
#test
{
    var x: String = "coucou㐁㐄㐅"
    y := XX(x)
    console.silent(true)
    console.write(y)
    console.silent(false)
}