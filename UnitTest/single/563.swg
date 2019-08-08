#module module563
//#unittest pass semantic
#unittest error

//#[swag.printbc]
struct point
{
	var x: s32 = 60
	var y: s32 = 70
}

impl point
{
	func opInit(self)
	{
	}
}