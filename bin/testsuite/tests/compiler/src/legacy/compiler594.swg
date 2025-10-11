#global testerror "Err0518"
func(op: string) toto() {}

#test
{
    toto'("0", "0")()
}
