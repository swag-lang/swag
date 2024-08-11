#global testerror "Err0482"
func toto(x: {a,b: s32})
{
}

#test
{
    toto({1})
}