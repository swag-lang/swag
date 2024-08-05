#global testerror "Err0594"
func toto(x: {a,b: s32})
{
}

#test
{
    toto({1})
}