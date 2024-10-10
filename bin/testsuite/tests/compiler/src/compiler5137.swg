#global testerror "Err0255"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}