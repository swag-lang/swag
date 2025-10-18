#global testerror "Err0432"
func(op: string) toto() {}

#test
{
    var c = 0
    toto'(c)()
}
