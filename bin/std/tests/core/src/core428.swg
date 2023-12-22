using Core

#test
{
    var str: String = "ABC"
    var cpt = 0
    visit,bytes,back v: str
    {
        if cpt == 0 do @assert(v == `C`)
        if cpt == 1 do @assert(v == `B`)
        if cpt == 2 do @assert(v == `A`)
        cpt += 1
    }
}