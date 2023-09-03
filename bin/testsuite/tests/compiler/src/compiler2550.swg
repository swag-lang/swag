#global testerror "Err0138"

func toto()
{
#if false throw "toto"
#else throw "toto"
}

#test
{
    toto()
}