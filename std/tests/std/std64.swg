#import "std"
using std.collections

struct X
{
}

impl X
{
    func opDrop(using self)
    {
        //@print("opDrop\n")
    }
}

//#[swag.printbc]
#test
{
    var arr: Array'X
}