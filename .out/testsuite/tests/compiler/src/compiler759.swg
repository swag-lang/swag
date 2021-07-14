#global testerror

//#[Swag.PrintBc]
struct Point
{
	x: f32
	y: f32
}

//#[Swag.PrintBc]
func Print(x: s64)
{
/*	@print(x)
	@print(" ")
	@print(x.y)
	@print("\n")*/
}

//#[Swag.PrintBc]
#test
{
	Print(s32)
}
