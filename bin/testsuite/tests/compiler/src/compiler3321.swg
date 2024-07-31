#global testerror "Err0744"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
