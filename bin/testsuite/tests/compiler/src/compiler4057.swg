func titi(x, y: ref s32, z: s32)
{
    x = y + z
}

#test
{
    toto  := 0
    toto1 := 1
    toto2 := 2
    titi(&toto, &toto1, toto2)
    @assert(toto == 3)
}