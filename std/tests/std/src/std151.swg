#import "std"
using std

//#[swag.printbc]
#test
{
    res := String.join("this", "is", "a", "string")
    @assert(res == "thisisastring")
    res1 := String.joinWith("--", "this", "is", "a", "string")
    @assert(res1 == "this--is--a--string")

    var arr: Array'string
    arr.add("this")
    arr.add("is")
    arr.add("another")
    res2 := String.joinWith(" ", @spread(arr.toSlice()))
    @assert(res2 == "this is another")
}