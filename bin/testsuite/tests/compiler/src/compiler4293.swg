#global testerror "Err0177"
#global testerror "Err0053"
func toto(x: rune){}

#test
{
    toto(-1)
}