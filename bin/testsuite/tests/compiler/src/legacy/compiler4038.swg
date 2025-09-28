//#global testerror
func(S=s32) toto()
{
    #assert(S == s32)
}

#test
{
    toto()
}