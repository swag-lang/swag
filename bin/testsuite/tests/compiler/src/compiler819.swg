func toto(x: {a,b: s32})
{
	@assert(x.item0 == 10)
	@assert(x.item1 == 20)
}

//#[Swag.PrintBc]
#test
{
	var aa = {10,20}
	toto(aa)
}
