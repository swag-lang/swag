

//#[swag.printbc]
struct Point
{
	x: f32
	y: f32
}

//#[swag.printbc]
func Print(x: s64)
{
/*	@print(x)
	@print(" ")
	@print(x.y)
	@print("\n")*/
}

//#[swag.printbc]
#unittest error
#test
{
	Print(s32)
}
