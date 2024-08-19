#global testerror "Err0500"
func toto(x: {a,b: s32})
{
}

#test
{
    toto({1})
}