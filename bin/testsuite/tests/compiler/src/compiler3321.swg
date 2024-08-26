#global testerror "Err0644"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
