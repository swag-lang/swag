#global testerror
func toto(x: {s32, s32})
{
}

#test
{
    toto(@{1,2,3})
}