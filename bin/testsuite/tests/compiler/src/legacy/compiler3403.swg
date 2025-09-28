//#global testerror
func(A) toto(x: {a:bool, b:A})
{
    #assert(A == *f32)
}

#test
{
    var x = 0'f32
    toto({true, &x})
}