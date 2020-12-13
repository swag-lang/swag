

struct Point
{
	x: s32
}

func new()->Point { return @{1}; }

#unittest error
//#[swag.printbc]
#test
{
	new().x = 1
}