#module module679
//#unittest pass syntax

struct Point
{
	var x: s32
	var y: s32
}

#[swag.printbc]
#test
{
	var pt : Point(1, 1)
}
