

//#[Swag.printbc]
func arithmeticMean(numbers: s32...)
{
	@assert(numbers[0] == 10)
	@assert(numbers[1] == 20)
	@assert(numbers[2] == 30)
}

//#[Swag.printbc]
#test
{
	arithmeticMean(10, 20, 30)
}