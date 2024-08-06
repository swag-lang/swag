#global testerror "Err0670"
func toto(x: s32... = 3)
{
    @print(x[0])
}

#test
{
    toto()
}