//#global testerror

var G: s32 = 666

//#[Swag.Inline]
//#[Swag.PrintBc]
func toto(a:s32 = G)
{
    @assert(a == 666)
}

//#[Swag.PrintBc]
#test
{
    toto()
}