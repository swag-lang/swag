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
    @assert(hasMember(tt, "toto"))
    @assert(!hasMember(tt, "toto1"))
}