#import "core"
using core.reflection

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