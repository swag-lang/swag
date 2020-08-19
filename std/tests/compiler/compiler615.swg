#module "compiler615"


//#[swag.printbc]
#unittest error
func(T) Print(x: T, y: T)
{
}

#test
{
	Print!(s32)(1, "1")
}