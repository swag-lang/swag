using core, core.StrConv

#test
{
    discard catch parse.getX64("00000000000000000ffffffffffffffff")
    @assert(@err == null)
    discard catch parse.getX64("ffffffffffffffff")
    @assert(@err == null)
    discard catch parse.getX64("c0deC0DEc0deC0DE")
    @assert(@err == null)
    discard catch parse.getX64("ffffffffffffffff0")
    @assert(@err != null)
    discard catch parse.getX64("toto")
    @assert(@err != null)

    discard catch parse.getB64("001111111111111111111111111111111111111111111111111111111111111111")
    @assert(@err == null)
    discard catch parse.getB64("1111111111111111111111111111111111111111111111111111111111111111")
    @assert(@err == null)
    discard catch parse.getB64("11111111111111111111111111111111111111111111111111111111111111110")
    @assert(@err != null)
    discard catch parse.getB64("toto")
    @assert(@err != null)

    discard catch parse.getU64("toto")
    @assert(@err != null)
    discard catch parse.getU64("18446744073709551615")
    @assert(@err == null)
    discard catch parse.getU64("18446744073709551616")
    @assert(@err != null)

    discard catch parse.getS64("toto")
    @assert(@err != null)
    discard catch parse.getS64("-")
    @assert(@err != null)
    discard catch parse.getS64("-9223372036854775808")
    @assert(@err == null)
    discard catch parse.getS64("9223372036854775808")
    @assert(@err != null)
}