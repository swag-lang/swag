#global testerror "Err0387"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}