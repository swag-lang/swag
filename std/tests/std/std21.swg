#import "std"
using std

func XX(v: Utf8)->[..]u8 => [v.buffer, v.length]

//#[swag.printbc]
#test
{
    var x: Utf8 = "coucou㐁㐄㐅"
    y := XX(x)
    console.write(y)
    //@print("XXXX")
}