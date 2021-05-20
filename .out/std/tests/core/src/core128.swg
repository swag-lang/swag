
using core, core.strconv

#test
{
    @assert(assume parse.getU64("255").value == 255)
    @assert(assume parse.getU64("123456").value == 123456)

    @assert(assume parse.getS64("255").value == 255)
    @assert(assume parse.getS64("-255").value == -255)
    @assert(assume parse.getS64("+255").value == 255)

    @assert(assume parse.getX64("ff").value == 255)
    @assert(assume parse.getX64("0A").value == 10)

    @assert(assume parse.getB64("11").value == 3)
    @assert(assume parse.getB64("0000101").value == 5)

    @assert(Math.isEqualEpsilon(assume parse.getF64("1.0").value, 1.0))
    @assert(Math.isEqualEpsilon(assume parse.getF64("1.1").value, 1.1))
    @assert(Math.isEqualEpsilon(assume parse.getF64("-128.456").value, -128.456, 0.001))
}