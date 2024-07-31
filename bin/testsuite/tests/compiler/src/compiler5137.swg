#global testerror "Err0390"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}