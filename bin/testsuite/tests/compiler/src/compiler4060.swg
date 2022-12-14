#test
{
    toto  := 0
    toto1 := 1
    toto2 := 2

    ref1 := ref &toto1
    ref2 := ref &toto2

    ref2 <<= 1
    @assert(ref2 == 4)
    @assert(ref2 << 1  == 8)
    @assert(ref2 >> 1  == 2)

    @assert(toto2 << toto1 == 8)
}