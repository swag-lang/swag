#global testerror "Err0593"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
