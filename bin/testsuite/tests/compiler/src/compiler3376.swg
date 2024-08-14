#global testerror "Err0545"
func toto(x: {a,b: s32})
{
}

#test
{
    toto({1,2,3})
}