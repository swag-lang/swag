#module "compiler2138"
#unittest error

var G: s32

//#[swag.inline]
func toto(a:s32 = G)
{

}

#test
{
    toto()
}