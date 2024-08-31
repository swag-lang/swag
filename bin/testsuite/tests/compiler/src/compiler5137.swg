#global testerror "Err0282"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}