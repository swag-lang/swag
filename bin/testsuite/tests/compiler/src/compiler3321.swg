#global testerror "Err0651"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
