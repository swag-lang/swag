using core

//#[Swag.printbc]
#test
{
    res := "A假CD"
    cpt := 0'u32
    utf8.visitRunes(|c, idx| res)
    {
        switch(idx)
        {
            case 0:  @assert(c == "A"'rune); cpt += 1
            case 1:  @assert(c == "假"'rune); cpt += 1
            case 2:  @assert(c == "C"'rune); cpt += 1
            case 3:  @assert(c == "D"'rune); cpt += 1
            default: @assert(false)
        }
    }

    @assert(cpt == 4)
}