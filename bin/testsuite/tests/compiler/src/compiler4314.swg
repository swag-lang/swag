#global testerror "Err0661"
func toto(x: s32... = 3)
{
    @print(x[0])
}

#test
{
    toto()
}