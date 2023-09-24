#global testerror "Err0807"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}