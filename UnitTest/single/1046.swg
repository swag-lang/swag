#module module1046

struct Point
{
	x: s32
}

func new()->Point { return {1}; }

//#[swag.printbc]
#test
{
	//new().x = 1
}