//#global testerror
func(N: s32, T) toto(v: [N] T) {}

#test
{
    var x10: [10] f32
    toto'(10, f32)(x10)
}
