func(T, N: u32) toto()
{
    //@print(@typeof(T).name)
    //@print(cast(s64) N)
}

#test
{
    toto'(s32, 4)()
    toto'(f32, 5)()
    namealias titi = toto'(f32, 6)
    titi()
}