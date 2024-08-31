#global testerror "Err0528"
func toto(x: {a,b: s32})
{
}

#test
{
    toto({1})
}