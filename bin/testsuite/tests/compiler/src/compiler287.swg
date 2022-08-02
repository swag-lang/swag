//#[Swag.PrintBc]
#test
{
	var a: bool = true;
	var b = &a;
	var c = dref b;
	@assert(c == true);
}

#test
{
	var a: s32 = 666;
	var b = &a;
	var c = dref b;
	@assert(c == 666);
}
