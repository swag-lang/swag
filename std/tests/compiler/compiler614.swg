#module "compiler614"


//#[swag.printbc]
#unittest error
func(T) Print(x: T)
{
}

#test
{
	Print!(string)(1)
}