#global testerror

#test
{
    try toto()
}

func toto()
{
    titi()
#if false throw "toto"
}

func titi()
{
#if false throw "toto"
}