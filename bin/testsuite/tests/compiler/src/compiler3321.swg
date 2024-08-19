#global testerror "Err0640"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
