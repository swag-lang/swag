using Core, Core.StrConv

#test
{
    discard catch Parse.getX64("00000000000000000ffffffffffffffff")
    @assert(@err == null)
    discard catch Parse.getX64("ffffffffffffffff")
    @assert(@err == null)
    discard catch Parse.getX64("c0deC0DEc0deC0DE")
    @assert(@err == null)
    discard catch Parse.getX64("ffffffffffffffff0")
    @assert(@err != null)
    discard catch Parse.getX64("toto")
    @assert(@err != null)

    discard catch Parse.getB64("001111111111111111111111111111111111111111111111111111111111111111")
    @assert(@err == null)
    discard catch Parse.getB64("1111111111111111111111111111111111111111111111111111111111111111")
    @assert(@err == null)
    discard catch Parse.getB64("11111111111111111111111111111111111111111111111111111111111111110")
    @assert(@err != null)
    discard catch Parse.getB64("toto")
    @assert(@err != null)

    discard catch Parse.getU64("toto")
    @assert(@err != null)
    discard catch Parse.getU64("18446744073709551615")
    @assert(@err == null)
    discard catch Parse.getU64("18446744073709551616")
    @assert(@err != null)

    discard catch Parse.getS64("toto")
    @assert(@err != null)
    discard catch Parse.getS64("-")
    @assert(@err != null)
    discard catch Parse.getS64("-9223372036854775808")
    @assert(@err == null)
    discard catch Parse.getS64("9223372036854775808")
    @assert(@err != null)
}