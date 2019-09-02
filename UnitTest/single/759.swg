#module module759

//#[swag.printbc]
struct Point
{
	var x: f32
	var y: f32
}

//#[swag.printbc]
func Print(x: s32)
{
/*	@print(x.x)
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
