struct Point
{
	x: s32 = 666
}

//#[Swag.PrintBc]
#test
{
	var pt: Point
	var slice : [..] *Point = @[&pt]
	@assert(slice[0].x == 666)

	var slice1 : [1] *Point = @[&pt]
	@assert(slice1[0].x == 666)
}