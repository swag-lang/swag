#import "std"
using std

//#[swag.printbc]
#test
{
    res := String.join("this", "is", "a", "string")
    @assert(res == "thisisastring")
    res1 := String.joinWith("--", "this", "is", "a", "string")
    @assert(res1 == "this--is--a--string")
}