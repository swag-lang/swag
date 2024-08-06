#global testerror "Err0369"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}