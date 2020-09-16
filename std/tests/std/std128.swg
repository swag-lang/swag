#import "std"
using std.collections, std.text

#test
{
    @assert(parseU64("255").value == 255)
    @assert(parseU64("123456").value == 123456)
}