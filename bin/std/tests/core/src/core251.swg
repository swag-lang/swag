using Core, Core.StrConv

#test
{
    discard catch parseX64("00000000000000000ffffffffffffffff")
    @assert(@err() == null)
    discard catch parseX64("ffffffffffffffff")
    @assert(@err() == null)
    discard catch parseX64("c0deC0DEc0deC0DE")
    @assert(@err() == null)
    var val2 = catch parseX64("ffffffffffffffff0")
    @assert(val2.ovf)
    discard catch parseX64("toto")
    @assert(@err() != null)

    discard catch parseB64("001111111111111111111111111111111111111111111111111111111111111111")
    @assert(@err() == null)
    discard catch parseB64("1111111111111111111111111111111111111111111111111111111111111111")
    @assert(@err() == null)
    var val3 = catch parseB64("11111111111111111111111111111111111111111111111111111111111111110")
    @assert(val3.ovf)
    discard catch parseB64("toto")
    @assert(@err() != null)

    discard catch parseU64("toto")
    @assert(@err() != null)
    discard catch parseU64("18446744073709551615")
    @assert(@err() == null)
    var val1 = catch parseU64("18446744073709551616")
    @assert(val1.ovf)

    discard catch parseS64("toto")
    @assert(@err() != null)
    discard catch parseS64("-")
    @assert(@err() != null)
    discard catch parseS64("-9223372036854775808")
    @assert(@err() == null)
    var val = catch parseS64("9223372036854775808")
    @assert(val.ovf)
}
