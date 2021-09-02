#global testerror
func toto();

var G = 0
func toto(x: s32)
{
    G = 1
}

#test
{
    toto()
}