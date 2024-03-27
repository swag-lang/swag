//#global testerror
func(S = s32, T = bool) toto()
{
    #assert S == s32
    #assert T == bool
}

#test
{
    toto()
}
