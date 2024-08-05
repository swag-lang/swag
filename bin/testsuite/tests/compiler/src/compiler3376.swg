#global testerror "Err0633"
func toto(x: {a,b: s32})
{
}

#test
{
    toto({1,2,3})
}