//#global testerror

func(T) toto()
{
#if false
    #if false
        throw "toto"
}

#test
{
    toto's32()
}