#module module691
//#unittest pass syntax

struct(T) Point
{
	var x: T = 5
	var y: T = 10
}

type Point2f: Point!f32

//#[swag.printbc]
#unittest error
#test
{
	var pt : Point2f(1.0, true)
}