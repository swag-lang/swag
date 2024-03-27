#global testerror "Err0717"
func(T) toto(x: *T)
{
    @print(t[0])
}

#test
{
    var cpt = 50
    toto's32(&cpt)
}
