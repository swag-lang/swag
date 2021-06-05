#global testerror

struct Point
{
	x: s32
}

func new()->Point { return @{1}; }

//#[Swag.printbc]
#test
{
	new().x = 1
}