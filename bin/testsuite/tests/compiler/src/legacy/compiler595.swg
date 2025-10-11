#global testerror "Err0592"
func(op: string) toto() {}

#test
{
    toto'(0)()
}
