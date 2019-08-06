#module module563
//#unittest pass semantic

//#[swag.printbc]
struct point
{
	var x: s32 = 60
	var y: s32 = 70
}

#unittest error
impl point
{
	func opInit(self)
	{
	}
}