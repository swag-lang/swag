#global testerror "Err0628"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
