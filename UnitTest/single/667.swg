#module module667
//#unittest pass syntax

type toto: s32

//#[swag.printbc]
#unittest error
#test
{
	a := 0
	if(a == toto)
	{
	}
}
