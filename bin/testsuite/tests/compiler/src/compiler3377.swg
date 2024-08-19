#global testerror "Err0497"
func toto(x: {a,b: s32})
{
}

#test
{
    toto({1})
}