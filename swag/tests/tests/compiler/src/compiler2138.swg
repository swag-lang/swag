
#global testerror

var G: s32

//#[swag.inline]
func toto(a:s32 = G)
{

}

#test
{
    toto()
}