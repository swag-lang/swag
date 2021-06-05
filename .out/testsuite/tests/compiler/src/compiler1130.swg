enum TOTO
{
	A
	B
}

//#[Swag.printbc]
#test
{
	@assert(@countof(TOTO) == 2)
}