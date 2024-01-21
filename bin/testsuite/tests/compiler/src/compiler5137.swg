#global testerror "Err0388"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}