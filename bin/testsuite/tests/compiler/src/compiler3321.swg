#global testerror "Err0733"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
