

//#[swag.printbc]
//#unittest error
func(T) Print(x: T)
{
	//@print(x)
}

#test
{
	Print'f32(1)
	Print("toto")
}