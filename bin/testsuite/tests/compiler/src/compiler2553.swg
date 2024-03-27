#global testerror "Err0712"

func toto()
{
    #if false #do func titi() => 666's64
    @print(titi())
}

#test
{
    toto()
}