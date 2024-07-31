#global testerror "Err0720"

func toto()
{
    #if false #do func titi() => 666's64
    @print(titi())
}

#test
{
    toto()
}