#test
{
    var toto  = 0
    var toto1 = 1
    var toto2 = 2

    var ref1 = #ref &toto1
    var ref2 = #ref &toto2

    ref2 <<= 1
    @assert(ref2 == 4)
    @assert(ref2 << 1 == 8)
    @assert(ref2 >> 1 == 2)

    @assert(toto2 << toto1 == 8)
}
