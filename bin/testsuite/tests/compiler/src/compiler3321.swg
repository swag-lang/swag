#global testerror "Err0625"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
