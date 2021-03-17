//#global testerror

func(T) toto()
{
#if false throw "toto"
}

#test
{
    toto's32()
}