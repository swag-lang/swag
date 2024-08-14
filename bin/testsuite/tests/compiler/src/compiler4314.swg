#global testerror "Err0653"
func toto(x: s32... = 3)
{
    @print(x[0])
}

#test
{
    toto()
}