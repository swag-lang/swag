#global testerror "Err0241"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}