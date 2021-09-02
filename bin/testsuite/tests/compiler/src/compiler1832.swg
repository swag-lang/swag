struct TOTO
{
    alias R = s32
}

#test
{
    var x: TOTO.R = 0
    @assert(@typeof(TOTO.R) == s32)
}