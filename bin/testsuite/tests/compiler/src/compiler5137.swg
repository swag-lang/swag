#global testerror "Err0376"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}