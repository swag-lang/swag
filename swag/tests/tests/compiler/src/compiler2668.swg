//#global testerror

var G: s32 = 666

#[swag.inline]
//#[swag.printbc]
func toto(a:s32 = G)
{
    @assert(a == 666)
}

//#[swag.printbc]
#test
{
    toto()
}