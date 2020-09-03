#import "std"
using std.text, std.collections

func cc(sl: [..] u8, val: s32)
{
    loop @countof(sl)
    {

    }
}

//#[swag.printbc]
#test
{
    cc("toto", 666)
//    var x: String = "toto"
//    var y: Array'u16 = x.toUnicode16()
}