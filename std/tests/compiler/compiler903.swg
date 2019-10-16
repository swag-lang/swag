#module compiler903

//#[swag.printbc]
#unittest error
func arithmeticMean(numbers: ...)
{
	cpt := cast(s32) numbers[1]
}

//#[swag.printbc]
#test
{
	arithmeticMean(10)
}