//#[Swag.PrintBc]
#global testerror "Err0652"
func(T) Print(x: T, y: T)
{
}

#test
{
	Print'(s32)(1, "1")
}