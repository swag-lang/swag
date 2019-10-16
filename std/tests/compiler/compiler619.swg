#module compiler619
//#unittest pass semantic

//#[swag.printbc]
#unittest error
func(x: string) Print()
{
	//@print(x)
}

#test
{
	Print()
}