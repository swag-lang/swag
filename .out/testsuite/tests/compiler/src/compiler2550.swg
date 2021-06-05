#global testerror

func toto()
{
#if false throw "toto"
#else throw "toto"
}

#test
{
    toto()
}