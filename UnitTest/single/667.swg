#module module667
//#unittest pass syntax

typealias toto: s32

//#[swag.printbc]
#unittest error
#test
{
	a := 0
	if(a == toto)
	{
	}
}
