#global testerror "Err0476"
#test
{
	//var a: (x: s32)->s32 = { return x; }
	var a: func(s32)->s32
	a()
}