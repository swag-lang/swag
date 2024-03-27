//#global skip
using Core

//#[Swag.PrintBc]
#test
{
    let res = "A假CD"
    var cpt = 0'u32
    Utf8.visitRunes(|c, idx| res)
    {
        switch(idx)
        {
            case 0:  @assert(c == `A`); cpt += 1
            case 1:  @assert(c == `假`); cpt += 1
            case 2:  @assert(c == `C`); cpt += 1
            case 3:  @assert(c == `D`); cpt += 1
            default: @assert(false)
        }
    }

    @assert(cpt == 4)
}