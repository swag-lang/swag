#global testerror "Err0602"
func toto(x: {a,b: s32})
{
}

#test
{
    toto({1})
}