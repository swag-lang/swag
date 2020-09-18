#import "std"
using std.collections, std.strconv

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

    @assert(math.isEqualEpsilon(parseF64("1.0").value, 1.0))
    @assert(math.isEqualEpsilon(parseF64("1.1").value, 1.1))
    @assert(math.isEqualEpsilon(parseF64("-128.456").value, -128.456, 0.001))
}