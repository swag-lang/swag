#module compiler664
//#unittest pass syntax

typealias toto: s32

//#[swag.printbc]
#unittest error
#test
{
	pt := toto
}
