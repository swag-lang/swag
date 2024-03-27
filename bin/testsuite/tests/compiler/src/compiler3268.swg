#global testerror "Err0649"
func(N: s32, T) toto(v: [N] T)
{
}

#test
{
    var x10: [10] f32
    toto'(10, s32)(x10)
}
