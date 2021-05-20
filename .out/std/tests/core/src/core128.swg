
using core, core.StrConv

#test
{
    @assert(assume Parse.getU64("255").value == 255)
    @assert(assume Parse.getU64("123456").value == 123456)

    @assert(assume Parse.getS64("255").value == 255)
    @assert(assume Parse.getS64("-255").value == -255)
    @assert(assume Parse.getS64("+255").value == 255)

    @assert(assume Parse.getX64("ff").value == 255)
    @assert(assume Parse.getX64("0A").value == 10)

    @assert(assume Parse.getB64("11").value == 3)
    @assert(assume Parse.getB64("0000101").value == 5)

    @assert(Math.isEqualEpsilon(assume Parse.getF64("1.0").value, 1.0))
    @assert(Math.isEqualEpsilon(assume Parse.getF64("1.1").value, 1.1))
    @assert(Math.isEqualEpsilon(assume Parse.getF64("-128.456").value, -128.456, 0.001))
}