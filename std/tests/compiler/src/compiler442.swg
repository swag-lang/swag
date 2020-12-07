#module "compiler442"
#unittest pass syntax

func F(a: s32, b: s32)
{
}

//#[swag.printbc]
#test
{
	F(a: 0, b: 1)
}