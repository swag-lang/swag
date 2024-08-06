#global testerror "Err0729"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
