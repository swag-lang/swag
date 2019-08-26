#module module669
//#unittest pass syntax

type toto: bool

//#[swag.printbc]
#unittest error
#test
{
	a := toto[0]
}
