#global testerror "Err0462"

struct Point
{
	x: f32
	y: f32
	z: f32
}

//#[Swag.PrintBc]
#test
{
   var pt : Point{y: 1.0, x: 1.0, 1.0}
}
