#global testerror "Err0676"
func toto(x: s32... = 3)
{
    @print(x[0])
}

#test
{
    toto()
}