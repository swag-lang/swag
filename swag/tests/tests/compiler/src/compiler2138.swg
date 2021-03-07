//#global testerror

var G: s32 = 666

//#[swag.inline]
func toto(a:s32 = G)
{
    @assert(a == 666)
}

#test
{
    toto()
}