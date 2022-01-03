#global testerror
func toto(a, b, c, d, e, f, g, h: s32)
{
}

#test
{
    ptr := @mkcallback(&toto)
}
