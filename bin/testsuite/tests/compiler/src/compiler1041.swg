#global testerror "Err0611"

//#[Swag.PrintBc]
#test
{
	var pt: s32 = 1
	let ptr = &pt
	@drop(ptr, 2)
}