#global testerror "Err0552"
func toto(x: {a,b: s32})
{
}

#test
{
    toto({1})
}