#import "std"
using std.strconv

//#[swag.printbc]
//#[swag.nooptim]
#test
{
    res := ascii.compare("toto", "TOTO")
    @assert(res > 0)
    res1 := ascii.compare("toto", "TOTO", true)
    @assert(res1 == 0)
}