#global testerror "Err0254"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}