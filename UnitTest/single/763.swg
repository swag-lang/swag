#module module763

struct Point
{
	var x: s32 = 666
	var y: s32
}

//#[swag.printbc]
#test
{	
	cpt := Point(10, 20)
	@print(cpt.x)
}
