#global testerror "Err0487"
#test
{
	//var a: (x: s32)->s32 = { return x; }
	var a: func(s32)->s32
	a()
}