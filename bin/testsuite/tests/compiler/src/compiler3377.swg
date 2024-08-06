#global testerror "Err0542"
func toto(x: {a,b: s32})
{
}

#test
{
    toto({1})
}