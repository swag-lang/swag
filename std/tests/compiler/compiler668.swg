#module compiler668
//#unittest pass syntax

typealias toto: bool

//#[swag.printbc]
#unittest error
#test
{
	a := !toto
}
