#global testerror "Err0326"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}