#module module536
#unittest pass semantic

struct point1
{
	var x = 100
	var y = 200
	var z = 300
}

struct point
{
	var in: point1
	var x = 10
	var y = 20
}

//#[swag.printbc]
#test
{
	var pt: point
	@assert(@op(pt, sizeof) == 20)
}