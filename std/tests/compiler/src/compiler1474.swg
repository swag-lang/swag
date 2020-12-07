#test
{
    cpt := 0
    visit xxx: "toto"
    {
        cpt += cast(s32) xxx
    }

    @assert(cpt == 454)
}