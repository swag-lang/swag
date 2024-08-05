#global testerror "Err0382"

func toto(x: ...) {}

#test
{
    toto(@spread(0))
}