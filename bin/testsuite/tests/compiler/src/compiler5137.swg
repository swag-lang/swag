#global testerror "Err0248"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}