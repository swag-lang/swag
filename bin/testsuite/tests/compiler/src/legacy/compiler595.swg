#global testerror "Err0603"
func(op: string) toto() {}

#test
{
    toto'(0)()
}
