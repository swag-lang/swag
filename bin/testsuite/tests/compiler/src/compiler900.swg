

//#[Swag.PrintBc]
func arithmeticMean(numbers: string...)
{
	@assert(numbers[0] == "10")
	@assert(numbers[1] == "20")
	@assert(numbers[2] == "30")
}

//#[Swag.PrintBc]
#test
{
	arithmeticMean("10", "20", "30")
}