using core, core.strconv

#test
{
    discard catch parseX64("00000000000000000ffffffffffffffff")
    @assert(@err == null)
    discard catch parseX64("ffffffffffffffff")
    @assert(@err == null)
    discard catch parseX64("c0deC0DEc0deC0DE")
    @assert(@err == null)
    discard catch parseX64("ffffffffffffffff0")
    @assert(@err != null)
    discard catch parseX64("toto")
    @assert(@err != null)

    discard catch parseB64("001111111111111111111111111111111111111111111111111111111111111111")
    @assert(@err == null)
    discard catch parseB64("1111111111111111111111111111111111111111111111111111111111111111")
    @assert(@err == null)
    discard catch parseB64("11111111111111111111111111111111111111111111111111111111111111110")
    @assert(@err != null)
    discard catch parseB64("toto")
    @assert(@err != null)

    discard catch parseU64("toto")
    @assert(@err != null)
    discard catch parseU64("18446744073709551615")
    @assert(@err == null)
    discard catch parseU64("18446744073709551616")
    @assert(@err != null)

    discard catch parseS64("toto")
    @assert(@err != null)
    discard catch parseS64("-")
    @assert(@err != null)
}