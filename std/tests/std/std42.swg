#import "std"
using std.math

#test
{
    var rect: Rect(0, 0, 10, 10)
    @assert(rect.right == 10)
}