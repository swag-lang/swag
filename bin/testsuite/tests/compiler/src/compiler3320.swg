#global testerror "Err0738"
func toto(a, b, c, d, e, f, g, h: s32)
{
}

#test
{
    var ptr = @mkcallback(&toto)
}
