#global testerror "Err0707"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
