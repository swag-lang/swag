#global testerror "Err0252"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}