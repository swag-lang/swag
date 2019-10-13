#module module614
//#unittest pass semantic

//#[swag.printbc]
#unittest error
func(T) Print(x: T)
{
}

#test
{
	Print!(string)(1)
}