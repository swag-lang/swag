#import "core"
using core, utf8

//#[swag.printbc]
#test
{
    res := "A假CD"
    cpt := 0'u32
    visit(Chars) c, idx: res
    {
        switch(idx)
        {
            case 0:  @assert(c == "A"'char); cpt += 1
            case 1:  @assert(c == "假"'char); cpt += 1
            case 2:  @assert(c == "C"'char); cpt += 1
            case 3:  @assert(c == "D"'char); cpt += 1
            default: @assert(false)
        }
    }

    @assert(cpt == 4)
}