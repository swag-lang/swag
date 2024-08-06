//#[Swag.PrintBc]
#global testerror "Err0602"
func(T) Print(x: T, y: T)
{
}

#test
{
	Print'(s32)(1, "1")
}