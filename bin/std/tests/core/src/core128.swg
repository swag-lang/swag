using Core, Core.StrConv

#test
{
    @assert(assume parseU64("255").value == 255)
    @assert(assume parseU64("123456").value == 123456)

    @assert(assume parseS64("255").value == 255)
    @assert(assume parseS64("-255").value == -255)
    @assert(assume parseS64("+255").value == 255)

    @assert(assume parseX64("ff").value == 255)
    @assert(assume parseX64("0A").value == 10)

    @assert(assume parseB64("11").value == 3)
    @assert(assume parseB64("0000101").value == 5)

    @assert(Math.isEqualEpsilon(assume parseF64("1.0").value, 1.0))
    @assert(Math.isEqualEpsilon(assume parseF64("1.1").value, 1.1))
    @assert(Math.isEqualEpsilon(assume parseF64("-128.456").value, -128.456, 0.001))
}
