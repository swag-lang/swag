//#global testerror
func(N: s32) toto(v: [N] f32)
{
    #assert(N == 10)
    #assert(#typeof(v) == #type [10] f32)
}

#test
{
    var x10: [10] f32
    toto'10(x10)
}