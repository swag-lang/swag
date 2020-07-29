#module compiler902
#unittest error

//#[swag.printbc]
func arithmeticMean(numbers: s32...)
{
	cpt := numbers[1]
}

//#[swag.printbc]
#test
{
	arithmeticMean(10)
}