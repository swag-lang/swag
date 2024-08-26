#global testerror "Err0281"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}