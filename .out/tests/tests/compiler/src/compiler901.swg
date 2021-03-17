//#[swag.printbc]
func arithmeticMean(numbers: const [..] string)
{
	@assert(numbers[0] == "10")
}

//#[swag.printbc]
#test
{
	arithmeticMean(@["10"])
}