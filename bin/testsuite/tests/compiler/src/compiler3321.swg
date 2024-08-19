#global testerror "Err0637"
func toto()->string
{
    return ""
}

#test
{
    var ptr = @mkcallback(&toto)
}
