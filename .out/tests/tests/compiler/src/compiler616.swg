

//#[swag.printbc]
//#global testerror
func(T, U) Print(x: T, y: U)
{
	//@print(x)
	//@print(y)
}

#test
{
	Print'(s32, f32)(1, 1.0)
}