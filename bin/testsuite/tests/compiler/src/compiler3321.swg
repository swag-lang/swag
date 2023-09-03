#global testerror "Err0786"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
