#global testerror "Err0321"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}