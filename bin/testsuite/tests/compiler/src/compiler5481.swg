#global testerror "Err0093"

struct Point
{
	x: s32
}

#[Swag.Inline]
func new()->Point { return {1}; }

//#[Swag.PrintBc]
#test
{
	new().x = 1
}