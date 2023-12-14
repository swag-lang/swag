using Core

#test
{
    var str: String = "ABC"
    var cpt = 0
    visit,bytes,back v: str
    {
        if cpt == 0 @assert(v == `C`)
        if cpt == 1 @assert(v == `B`)
        if cpt == 2 @assert(v == `A`)
        cpt += 1
    }
}