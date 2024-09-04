#global testerror "Err0668"

func toto()
{
    #if false: func titi() => 666's64
    @print(titi())
}

#test
{
    toto()
}