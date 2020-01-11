#import "std"
using std.collections

struct BOUBOU
{
    value: s32 = 666
    value1: s32 = 666
}

impl BOUBOU
{
    func opDrop(using self)
    {
        @print("opDrop65 ")
    }
}

#[swag.printbc]
#test
{
    var arr: Array'BOUBOU
    var value: BOUBOU
    arr.addBack(value)
}