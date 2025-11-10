#test
{
    var cpt = 0
    foreach xxx in "toto"
    {
        cpt += cast(s32) xxx
    }

    @assert(cpt == 454)
}
