#global testerror "Err0594"
func(op: string) toto() {}

#test
{
    toto'("0", "0")()
}
