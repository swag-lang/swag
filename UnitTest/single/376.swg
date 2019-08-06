#module module376;
//#unittest pass semantic;

//#[swag.printbc]
func a (ptr: string, cpt: s32)
{
	@assert(cpt == 10);
	@assert(ptr[0] == cast(u8) ('t'));
	@assert(ptr[3] == cast(u8) ('o'));
}

//#[swag.printbc]
#test
{
	a("toto", 10);
}
