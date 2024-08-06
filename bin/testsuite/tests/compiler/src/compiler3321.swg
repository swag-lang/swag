#global testerror "Err0657"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
