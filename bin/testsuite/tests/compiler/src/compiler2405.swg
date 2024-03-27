func(T, N: u32) toto()
{
}

#test
{
    toto'(s32, 4)()
    toto'(f32, 5)()
    namealias titi = toto'(f32, 6)
    titi()
}
