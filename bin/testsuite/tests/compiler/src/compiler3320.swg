#global testerror "Err0498"
func toto(a, b, c, d, e, f, g, h, i, j, k: s32)
{
}

#test
{
    var ptr = @mkcallback(&toto)
}
