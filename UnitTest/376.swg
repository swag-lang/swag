#module module376;
//#unittest pass semantic;

//#[swag.printbytecode]
func a: (ptr: string, cpt: s32)
{
	@assert(cpt == 10);
}

//#[swag.printbytecode]
#test
{
	a("toto", 10);
}
