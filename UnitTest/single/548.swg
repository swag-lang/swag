#module module548
#unittest pass semantic

struct point
{
	var x = 10
	var y = 20
}

#[swag.printbytecode]
#test
{
	var pt: [10] point
	//@print(pt[0].x)
}