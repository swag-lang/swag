#test
{
    var cpt = 0
    visit xxx in "toto"
    {
        cpt += cast(s32) xxx
    }

    @assert(cpt == 454)
}