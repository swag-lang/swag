#module module901

//#[swag.printbc]
func arithmeticMean(numbers: [..] string)
{
	@assert(numbers[0] == "10")
}

//#[swag.printbc]
#test
{
	arithmeticMean(["10"])
}