#global testerror "Err0320"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}