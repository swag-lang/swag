#import "std"
using std.reflection

struct X
{

}

impl X
{
    func toto()
    {

    }
}

//#[swag.printbc]
#test
{
    tt := @typeof(X)
    @assert(hasMethod(tt, "toto"))
    @assert(!hasMethod(tt, "toto1"))
}