#module module648
//#unittest pass semantic

func(T) toto()
{
	var x: T = 0
}

//#[swag.printbc]
//#unittest error
#test
{
	toto!s32()
}
