#import "std"
using std.collections, std.text

#test
{
    @assert(parseU64("255").value == 255)
    @assert(parseU64("123456").value == 123456)
    @assert(parseS64("255").value == 255)
    @assert(parseS64("-255").value == -255)
    @assert(parseS64("+255").value == 255)

    @assert(parseX64("ff").value == 255)
    @assert(parseX64("0A").value == 10)

    @assert(parseB64("11").value == 3)
    @assert(parseB64("0000101").value == 5)
}