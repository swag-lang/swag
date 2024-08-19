#global testerror "Err0587"

//#[Swag.PrintBc]
#test
{
	var pt: s32 = 1
	let ptr = &pt
	@drop(ptr, 2)
}