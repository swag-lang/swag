//#[swag.printbc]
func arithmeticMean(numbers: ...)
{
    @assert(cast(string) numbers[0] == "10")
}

//#[swag.printbc]
#test
{
	arithmeticMean("10")
}