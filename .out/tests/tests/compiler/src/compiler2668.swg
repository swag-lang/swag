//#global testerror

var G: s32 = 666

#[Swag.inline]
//#[Swag.printbc]
func toto(a:s32 = G)
{
    @assert(a == 666)
}

//#[Swag.printbc]
#test
{
    toto()
}