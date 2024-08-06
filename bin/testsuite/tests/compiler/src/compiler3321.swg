#global testerror "Err0734"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
