#global testerror "Err0325"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}