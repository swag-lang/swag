#global testerror "Err0480"
func toto(x: {a,b: s32})
{
}

#test
{
    toto({1})
}