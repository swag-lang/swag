#import "std"
using std

func cc(sl: const [..] u8, val: s32)
{
    loop @countof(sl)
    {

    }
}

//#[swag.printbc]
#test
{
    std.debug.assert(true, "")
    cc("toto", 666)
//    var x: String = "toto"
//    var y: Array'u16 = x.toUnicode16()
}