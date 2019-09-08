#module module786

enum TOTO
{
	R = 100
	G = 200
	B = 300
}

//#[swag.printbc]
#test
{
	using swag
	
	t := @typeof(TOTO)
}