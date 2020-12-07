#import "std"
using std

//#[swag.printbc]
#test
{
    x := math.min'f32(1, 2)
    @assert(x == 1)
}