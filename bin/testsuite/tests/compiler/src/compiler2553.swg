#global testerror "Err0694"

func toto()
{
    #if false #do func titi() => 666's64
    @print(titi())
}

#test
{
    toto()
}