//#[Swag.PrintBc]
#global testerror "Syn0066"
#test
{
	//var a: (x: s32)->s32 = { return x; }
	var a: (s32)->s32
	discard a(0)
}