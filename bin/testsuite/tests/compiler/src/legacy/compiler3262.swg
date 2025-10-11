#global testerror "Err0193"
func(N: s32) toto(v: [N] f32) {}

#test
{
    var x10: [10] f32
    toto'5(x10)
}
