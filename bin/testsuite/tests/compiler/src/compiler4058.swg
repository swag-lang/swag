#test
{
    let toto  = 0
    var toto1 = 1
    var toto2 = 2

    let ref1 = #ref &toto1
    let ref2 = #ref &toto2

    @assert(ref1 + 1 == 2)
    @assert(ref1 + ref2 == 3)
    @assert(1 + ref2 == 3)
}
