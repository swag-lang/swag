#module module521
#unittest pass semantic

//#[swag.printbytecode]
struct point
{
	var x, y: f32
}

#test
{
	var pt: point
	pt.x = 10
}
