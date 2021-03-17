#global testerror

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
#test
{
	Print(s32)
}
