#global testerror "Err0679"

func toto()
{
    #if false: func titi() => 666's64
    @print(titi())
}

#test
{
    toto()
}