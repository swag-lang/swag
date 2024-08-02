#global testerror "Err0381"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}