#global testerror "Err0737"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
