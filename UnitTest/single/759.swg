#module module759

//#[swag.printbc]
struct Point
{
	var x: f32
	var y: f32
}

#[swag.printbc]
func Print(x: Point)
{
/*	@print(x.x)
	@print(" ")
	@print(x.y)
	@print("\n")*/
}

#[swag.printbc]
#test
{	
	Print(Point(1, 1))
}
