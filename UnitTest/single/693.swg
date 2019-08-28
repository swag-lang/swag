#module module693
//#unittest pass syntax

struct(T) Point
{
	var x: T = 5
	var y: T = 10
}

#unittest error
type Point2f: Point
