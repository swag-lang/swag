#global testerror "Err0755"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
