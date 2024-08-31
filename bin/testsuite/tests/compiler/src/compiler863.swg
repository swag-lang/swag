//#[Swag.PrintBc]
#global testerror "Err0289"
#test
{
	//var a: (x: s32)->s32 = { return x; }
	var a: (s32)->s32
	discard a(0)
}