#global testerror "Err0720"
func(op: string) toto() {}

#test
{
    var c = 0
    toto'(c)()
}
