#global testerror "Err0556"
func toto(x: {a,b: s32})
{
}

#test
{
    toto({1,2,3})
}