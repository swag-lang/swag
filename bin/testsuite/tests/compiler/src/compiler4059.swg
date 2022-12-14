#test
{
    toto  := 0
    toto1 := 1
    toto2 := 2

    ref1 := ref &toto1
    ref2 := ref &toto2

    @assert(-ref1 == -1)
}