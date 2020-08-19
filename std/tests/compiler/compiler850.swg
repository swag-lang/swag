#module "compiler850"

#unittest error
struct Point
{
}

impl Point
{
	func opDrop(self, x: s32)
	{
	}
}

//#[swag.printbc]
#test
{
}