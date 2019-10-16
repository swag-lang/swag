#module compiler902

//#[swag.printbc]
#unittest error
func arithmeticMean(numbers: s32...)
{
	cpt := numbers[1]
}

//#[swag.printbc]
#test
{
	arithmeticMean(10)
}