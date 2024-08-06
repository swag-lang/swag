#global testerror "Err0097"

struct Point
{
	x: s32
}

//#[Swag.PrintBc]
#test
{
	var pt: Point
	var ptr: const *Point = &pt
	ptr.x = 0
}