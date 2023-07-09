#global testerror
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
