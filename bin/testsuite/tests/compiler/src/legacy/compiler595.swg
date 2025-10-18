#global testerror "Err0444"
func(op: string) toto() {}

#test
{
    toto'(0)()
}
