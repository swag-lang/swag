#global testerror "Err0736"
func(op: string) toto() {}

#test
{
    toto'("0", "0")()
}
