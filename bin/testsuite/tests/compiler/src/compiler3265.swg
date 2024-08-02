#global testerror "Err0296"
func(N: s32) toto(v, w: [N] f32)
{
}

#test
{
    var x10: [10] f32
    var x11: [11] f32
    toto'10(x10, x11)
}