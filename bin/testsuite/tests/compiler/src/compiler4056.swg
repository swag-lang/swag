func titi(x, y, z: ref s32)
{
    x = y + z
}

#test
{
    toto  := 0
    toto1 := 1
    toto2 := 2
    titi(&toto, &toto1, &toto2)
    @assert(toto == 3)
}