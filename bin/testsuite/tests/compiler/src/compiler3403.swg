//#global testerror
func(A) toto(x: {bool, A})
{
    #assert A == *f32
}

#test
{
    x := 0'f32
    toto(@{true, &x})
}