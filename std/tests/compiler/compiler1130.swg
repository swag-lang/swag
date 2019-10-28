enum TOTO
{
	A
	B
}

//#[swag.printbc]
#test
{
	@assert(@countof(TOTO) == 2)
}