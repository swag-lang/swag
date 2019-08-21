#module module613
//#unittest pass semantic

//#[swag.printbc]

func(T) Print(x: T)
{
	@print(x)
}

#test
{
	Print!(s32)(1)
	Print!(f32)(1.0)
}