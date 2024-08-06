#global testerror "Err0706"

func toto()
{
    #if false #do func titi() => 666's64
    @print(titi())
}

#test
{
    toto()
}