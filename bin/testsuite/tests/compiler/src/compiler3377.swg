#global testerror "Err0583"
func toto(x: {a,b: s32})
{
}

#test
{
    toto({1})
}