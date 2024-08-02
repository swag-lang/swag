#global testerror "Err0728"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
