//#global testerror
func(N: s32) toto(v, w: [N] f32)
{
}

#test
{
    var x10: [10] f32
    toto'10(x10, x10)
}