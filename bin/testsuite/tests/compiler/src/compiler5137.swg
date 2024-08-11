#global testerror "Err0242"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}