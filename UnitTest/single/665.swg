#module module665
//#unittest pass syntax

type toto: s32

//#[swag.printbc]
#test
{
	var a: toto = 0
	var b: toto = 1
	a = b
	if(a == b)
	{
	}
}
