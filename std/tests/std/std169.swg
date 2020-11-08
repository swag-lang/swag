#import "std"
using std.time

//#[swag.printbc]
func toto()->String
{
    var res: retval
    res = "toto"
    return res
}

#test
{
    v := toto()
    @assert(v == "toto")
}