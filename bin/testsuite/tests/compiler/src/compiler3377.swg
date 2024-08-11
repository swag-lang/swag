#global testerror "Err0487"
func toto(x: {a,b: s32})
{
}

#test
{
    toto({1})
}