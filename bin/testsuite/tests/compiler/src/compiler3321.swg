#global testerror "Err0629"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
