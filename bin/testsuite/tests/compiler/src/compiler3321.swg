#global testerror "Err0740"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
