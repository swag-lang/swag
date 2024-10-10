#global testerror "Err0605"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
