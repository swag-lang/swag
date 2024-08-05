#global testerror "Err0677"
func toto(x: s32... = 3)
{
    @print(x[0])
}

#test
{
    toto()
}