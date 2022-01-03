#global testerror
func toto()->string
{
    return ""
}

#test
{
    ptr := @mkcallback(&toto)
}
