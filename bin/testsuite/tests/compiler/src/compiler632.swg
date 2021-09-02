


struct(T) Point
{
	x: T
	y: T
}

#test
{
	var pt1: Point'(f32)
	var pt2: Point'(string)
	pt2.x = "0"
}