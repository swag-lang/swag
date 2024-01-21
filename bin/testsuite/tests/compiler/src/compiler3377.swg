#global testerror "Err0597"
func toto(x: {a,b: s32})
{
}

#test
{
    toto({1})
}