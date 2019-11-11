#import "std"
using std

func XX(v: Utf8)->[..]u8 => [v.buffer, v.size]

//#[swag.printbc]
#test
{
    var x: Utf8 = "coucou"
    y := XX(x)
    @print(y)
}